#include <fcntl.h>
#include "imgui_skia_app_sdl3.h"

#include <cstdio>
#include <cstring>

#include "imgui_impl_sdl3.h"
#include "imgui_internal.h"

#include <SDL3/SDL_main.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL.h>

#endif

#include "include/core/SkGraphics.h"
#include "include/ports/SkFontMgr_data.h"
#if defined(SK_FONTMGR_FONTCONFIG_AVAILABLE)
#include "include/ports/SkFontMgr_fontconfig.h"
#endif
#if defined(SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE)
#include "include/ports/SkFontMgr_directory.h"
#endif
#include "include/core/SkSpan.h"
#include "include/svg/SkSVGCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/encode/SkPngEncoder.h"
#include "include/encode/SkJpegEncoder.h"
#include "include/encode/SkWebpEncoder.h"
#if defined(__linux__)
#include "include/gpu/gl/glx/GrGLMakeGLXInterface.h"
#endif

#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "src/gpu/ganesh/gl/GrGLDefines.h"
#include "src/gpu/ganesh/gl/GrGLUtil.h"

#include "tracy/Tracy.hpp"

#include "flatbuffers/util.h"
#include "ImZeroFB.out.h"

template <typename T>
static void applyFlag(int &flag,T val,bool v) {
    if(v) {
        flag |= val;
    } else {
        flag &= ~(val);
    }
}
constexpr int msaaSampleCount = 0; //4;
constexpr int stencilBits = 8;  // Skia needs 8 stencil bits

void App::drawImGuiVectorCmdsFB(SkCanvas &canvas) { ZoneScoped;
    const ImDrawData* drawData = ImGui::GetDrawData();
    fTotalVectorCmdSerializedSize = 0;
    for (int i = 0; i < drawData->CmdListsCount; ++i) {
        ImDrawList* drawList = drawData->CmdLists[i];
        const uint8_t *buf;
        size_t sz;
        drawList->serializeFB(buf,sz);
        fTotalVectorCmdSerializedSize += sz;
        fVectorCmdSkiaRenderer.prepareForDrawing();
        fVectorCmdSkiaRenderer.drawSerializedVectorCmdsFB(buf, canvas);
    }
}

void App::paint(SkSurface* surface, int width, int height) { ZoneScoped;
    ImGui::useVectorCmd = fUseVectorCmd && surface != nullptr;
    auto renderMode = fVectorCmdSkiaRenderer.getRenderMode();
    ImGui::ShowDemoWindow();

    SaveFormatE saveFormat = SaveFormatE_None;
    if(ImGui::Begin("ImZeroSkia Settings")) { ZoneScoped;
        fImZeroSkiaSetupUi.render(saveFormat, fVectorCmdSkiaRenderer, fUseVectorCmd,
                                  fTotalVectorCmdSerializedSize, 0,
                                  fSkpBytesWritten, fSvgBytesWritten, fPngBytesWritten,
                                  width, height, fFontMgr.get()
        );
    }
    ImGui::End();

    ImGui::Render();

    if(surface == nullptr) {
        return;
    }

    if(saveFormat != SaveFormatE_None) {
        switch(saveFormat) {
            case SaveFormatE_SKP: { ZoneScoped;
                constexpr auto path = "/tmp/skiaBackend.skp";

                SkPictureRecorder skiaRecorder;
                auto skiaCanvas = skiaRecorder.beginRecording(SkIntToScalar(width),
                                                              SkIntToScalar(height));

                skiaCanvas->clear(fBackgroundColor);
                skiaCanvas->save();
                drawImGuiVectorCmdsFB(*skiaCanvas);
                skiaCanvas->restore();

                sk_sp<SkPicture> picture = skiaRecorder.finishRecordingAsPicture();
                SkFILEWStream skpStream(path);
                picture->serialize(&skpStream);
                fSkpBytesWritten = skpStream.bytesWritten();
                break;
            }
            case SaveFormatE_SVG: // fallthrough
            case SaveFormatE_SVG_TextAsPath: { ZoneScoped;
                SkRect bounds = SkRect::MakeIWH(width, height);
                fVectorCmdSkiaRenderer.changeRenderMode(renderMode | RenderModeE_SVG);

                switch(saveFormat) {
                    case SaveFormatE_SVG:
                    {
                        constexpr auto path1 = "/tmp/skiaBackend.svg";
                        constexpr int flags1 = SkSVGCanvas::kNoPrettyXML_Flag;
                        SkFILEWStream svgStream(path1);
                        { // svg canvas may buffer commands, extra scope to ensure flush by RAII
                            auto skiaCanvas = SkSVGCanvas::Make(bounds, &svgStream, flags1);
                            drawImGuiVectorCmdsFB(*skiaCanvas);
                        }
                        fSvgBytesWritten = svgStream.bytesWritten();
                    }
                        break;
                    case SaveFormatE_SVG_TextAsPath:
                    {
                        constexpr auto path = "/tmp/skiaBackend.nofont.svg";
                        constexpr int flags = SkSVGCanvas::kConvertTextToPaths_Flag | SkSVGCanvas::kNoPrettyXML_Flag;
                        SkFILEWStream svgStream(path);
                        { // svg canvas may buffer commands, extra scope to ensure flush by RAII
                            auto skiaCanvas = SkSVGCanvas::Make(bounds, &svgStream, flags);
                            drawImGuiVectorCmdsFB(*skiaCanvas);
                        }
                        fSvgBytesWritten = svgStream.bytesWritten();
                    }
                        break;
                    default:
                        ;
                }

                fVectorCmdSkiaRenderer.changeRenderMode(renderMode);
                break;
            }
            case SaveFormatE_PNG: { ZoneScoped;
                constexpr auto path = "/tmp/skiaBackend.png";
                const auto s = SkISize::Make(width, height);
                const auto c = SkColorInfo(kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
                sk_sp<SkSurface> rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(s, c));
                SkCanvas *rasterCanvas = rasterSurface->getCanvas();
                drawImGuiVectorCmdsFB(*rasterCanvas);
                sk_sp<SkImage> img(rasterSurface->makeImageSnapshot());
                SkFILEWStream pngStream(path);
                SkPixmap pixmap;
                fPngBytesWritten = 0;
                if(img->peekPixels(&pixmap)) {
                    if (SkPngEncoder::Encode(static_cast<SkWStream *>(&pngStream), pixmap, SkPngEncoder::Options{})) {
                        fPngBytesWritten = pngStream.bytesWritten();
                    }
                }
                break;
            }
            case SaveFormatE_VECTORCMD: { ZoneScoped;
                constexpr auto path = "/tmp/skiaBackend.flatbuffers";
                SkFILEWStream stream(path);

                const ImDrawData* drawData = ImGui::GetDrawData();
                fTotalVectorCmdSerializedSize = 0;
                for (int i = 0; i < drawData->CmdListsCount; ++i) {
                    ImDrawList* drawList = drawData->CmdLists[i];
                    const uint8_t *buf;
                    size_t sz;
                    drawList->serializeFB(buf,sz);
                    stream.write(buf,sz);
                }
                break;
            }
        }
    } else { ZoneScoped;
        auto skiaCanvas = surface->getCanvas();
        skiaCanvas->clear(fBackgroundColor);

        skiaCanvas->save();
        drawImGuiVectorCmdsFB(*skiaCanvas);
        skiaCanvas->restore();
    }

    FrameMark;
}
static void build_ImFontAtlas(ImFontAtlas& atlas, SkPaint& fontPaint) {
    int w, h;
    unsigned char* pixels;
    atlas.GetTexDataAsAlpha8(&pixels, &w, &h);
    SkImageInfo info = SkImageInfo::MakeA8(w, h);
    SkPixmap pmap(info, pixels, info.minRowBytes());
    SkMatrix localMatrix = SkMatrix::Scale(1.0f / static_cast<float>(w), 1.0f / static_cast<float>(h));
    auto fontImage = SkImages::RasterFromPixmap(pmap, nullptr, nullptr);
    auto fontShader = fontImage->makeShader(SkSamplingOptions(SkFilterMode::kLinear), localMatrix);
    fontPaint.setShader(fontShader);
    fontPaint.setColor(SK_ColorWHITE);
    ImTextureID texId;
    texId = reinterpret_cast<intptr_t>(&fontPaint);
    atlas.TexID = texId;
}

int App::Run(CliOptions &opts) {
    // prevent SIGPIPE when writing frames or reading user interaction events
    //signal(SIGPIPE, SIG_IGN);

    sk_sp<SkTypeface> typeface = nullptr;
    sk_sp<SkData> ttfData = nullptr;
    { // setup skia/imgui shared objects

        if (opts.fBackgroundColorRGBA != nullptr && strlen(opts.fBackgroundColorRGBA) == 8) {
            auto const n = static_cast<uint32_t>(strtoul(opts.fBackgroundColorRGBA, nullptr, 16));
            uint8_t a = n & 0xff;
            uint8_t b = (n >> 8) & 0xff;
            uint8_t g = (n >> 16) & 0xff;
            uint8_t r = (n >> 24) & 0xff;
            fBackgroundColor = SkColorSetARGB(a, r, g, b);
        }

        fUseVectorCmd = opts.fVectorCmd;
        ImGui::useVectorCmd = fUseVectorCmd;

        {
            ttfData = SkData::MakeFromFileName(opts.fTtfFilePath);
            if (ttfData == nullptr || ttfData->isEmpty()) {
                fprintf(stderr, "unable to open ttf file %s\n", opts.fTtfFilePath);
                exit(1);
            }
        }
        {
            fFontMgr = nullptr;

            if (opts.fFontManager != nullptr && strcmp(opts.fFontManager, "fontconfig") == 0) {
#if defined(SK_FONTMGR_FONTCONFIG_AVAILABLE)
                fFontMgr = SkFontMgr_New_FontConfig(nullptr);
#else
                fprintf(stderr,"SK_FONTMGR_FONTCONFIG_AVAILABLE is not defined, font manager %s not supported\n",opts.fontManager);
#endif
            }
            if (opts.fFontManager != nullptr && strcmp(opts.fFontManager, "directory") == 0) {
#if defined(SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE)
                fFontMgr = SkFontMgr_New_Custom_Directory(opts.fFontManagerArg);
#else
                fprintf(stderr,"SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE is not defined, font manager %s not supported\n",opts.fontManager);
#endif
            }
            if (fFontMgr == nullptr) {
                // fallback
                fprintf(stderr, "using fallback font manager (opt=%s,arg=%s)\n", opts.fFontManager, opts.fFontManagerArg);
                fFontMgr = SkFontMgr_New_Custom_Data(SkSpan(&ttfData, 1));
            }
        }
        typeface = fFontMgr->makeFromData(ttfData);
        ////auto const typeface = fontMgr->matchFamilyStyle(nullptr,SkFontStyle());
        if (typeface == nullptr || fFontMgr->countFamilies() <= 0) {
            fprintf(stderr, "unable to initialize font manager with supplied ttf font file %s\n", opts.fTtfFilePath);
            return (1);
        }

        ImGui::skiaFontDyFudge = opts.fFontDyFudge;
        ImGui::paragraph = std::make_shared<Paragraph>(fFontMgr, typeface);
        ImGui::paragraph->enableFontFallback();
        ImGui::skiaFont = SkFont(typeface);

        fVectorCmdSkiaRenderer.setVertexDrawPaint(&fFontPaint);
        fVectorCmdSkiaRenderer.setParagraphHandler(ImGui::paragraph);
        RenderModeE mode = 0;
        if (opts.fBackdropFilter) {
            mode |= RenderModeE_BackdropBlur;
        }
        if (opts.fSketchFilter) {
            mode |= RenderModeE_Sketch;
        }
        fVectorCmdSkiaRenderer.changeRenderMode(mode);
    }
    SDL_GLContext glContext = nullptr;
    const SDL_DisplayMode *dm = nullptr;

    // Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencilBits);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    if (msaaSampleCount > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaaSampleCount);
    }

    // Enable native IME.
    //SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    dm = SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());
    fWindow = SDL_CreateWindow(opts.fAppTitle, dm->w, dm->h, window_flags);
    if (fWindow == nullptr) {
        fprintf(stderr, "Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetWindowPosition(fWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    glContext = SDL_GL_CreateContext(fWindow);
    SDL_GL_MakeCurrent(fWindow, glContext);
    SDL_GL_SetSwapInterval(opts.fVsync ? 1 : 0); // Enable vsync //SDL_SetWindowSurfaceVSync()
    if (opts.fFullscreen) {
        SDL_SetWindowFullscreen(fWindow, SDL_WINDOW_FULLSCREEN);
    }
    SDL_ShowWindow(fWindow);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImVec4 clearColor;
    ImGuiIO &io = ImGui::GetIO();
    {
        applyFlag(io.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard, opts.fImguiNavKeyboard);
        applyFlag(io.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad, opts.fImguiNavGamepad);
        applyFlag(io.ConfigFlags, ImGuiConfigFlags_DockingEnable, opts.fImguiDocking);

        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // FIXME remove ?
        if (true) {
            io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        } else {
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
            io.ConfigViewportsNoAutoMerge = true;
            io.ConfigViewportsNoTaskBarIcon = true;
        }

        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        if(glContext != nullptr) {
            ImGui_ImplSDL3_InitForOpenGL(fWindow, glContext);
        }

        clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        io.ConfigInputTrickleEventQueue = true;
        io.ConfigWindowsMoveFromTitleBarOnly = false; // make config option
    }


    if(dm != nullptr) {
        createContext(clearColor, dm->w, dm->h);
    }

    build_ImFontAtlas(*io.Fonts, fFontPaint);

    return mainLoop(opts,glContext,clearColor);
}

App::~App() = default;
int App::mainLoop(CliOptions &opts,SDL_GLContext glContext,ImVec4 const &clearColor) {
    ImGuiIO &io = ImGui::GetIO();

    // Main loop
    bool done = false;
    while (!done) {
        auto const width = static_cast<int>(io.DisplaySize.x);
        auto const height = static_cast<int>(io.DisplaySize.y);

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            switch(event.type) {
                case SDL_EVENT_QUIT:
                    done = true;
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    // FIXME only necessary for multi-window setting?
                    if(event.window.windowID == SDL_GetWindowID(fWindow)) {
                        done = true;
                    }
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    destroyContext();
                    createContext(clearColor,width,height);
                    break;
            }
        }
        if(SDL_GetWindowFlags(fWindow) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        auto const surface = getSurfaceGL();

        // Start the Dear ImGui frame
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if(width <= 0 || height <= 0) {
           ImGui::Render();
	    } else {
            ImGui::ShowMetricsWindow();
            paint(surface.get(),width,height); // will call ImGui::Render();
            fContext->flush();

            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
            //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
                SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
            }

            SDL_GL_SwapWindow(fWindow);
        }
    }

    // Cleanup
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    destroyContext();

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(fWindow);
    SDL_Quit();
    return 0;
}

App::App() {
    fTotalVectorCmdSerializedSize = 0;
    fSkpBytesWritten = 0;
    fSvgBytesWritten = 0;
    fPngBytesWritten = 0;
    fBackgroundColor = SK_ColorRED;
    fUseVectorCmd = false;
    fFontMgr = nullptr;
    fFontPaint = SkPaint();
}

void App::createContext(ImVec4 const &clearColor,int width,int height) {
    glViewport(0, 0, width, height);
    glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    fNativeInterface = GrGLInterfaces::MakeGLX();
    //nativeInterface->checkAndResetOOMed();
    if(fNativeInterface == nullptr || !fNativeInterface->validate()) {
        fprintf(stderr, "unable to create skia GrGLInterface (GLX): nativeInterface=%p (%s)\n",
                fNativeInterface.get(),
                fNativeInterface->validate() ? "valid" : "invalid");
        exit(1);
    }
    fContext = GrDirectContexts::MakeGL(fNativeInterface);
    if(fContext == nullptr) {
        fprintf(stderr,"unable to create skia GrDirectContext (GL)\n");
        exit(1);
    }
}
sk_sp<SkSurface> App::getSurfaceRaster(int w, int h) {
    if(fSurface == nullptr) {
        constexpr SkColorType colorType = kRGBA_8888_SkColorType;
        constexpr SkAlphaType alphaType = kPremul_SkAlphaType;
        sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
        const auto c = SkColorInfo(colorType, alphaType, colorSpace);
        fSurface = SkSurfaces::Raster(SkImageInfo::Make(SkISize::Make(w,h), c));
    }
    return fSurface;
}
sk_sp<SkSurface> App::getSurfaceGL() {
    if(fSurface == nullptr && fContext != nullptr && fWindow != nullptr && fNativeInterface != nullptr) {
        // Wrap the frame buffer object attached to the screen in a Skia render target so Skia can render to it
        GrGLint buffer = 0;
        GR_GL_GetIntegerv(fNativeInterface.get(), GR_GL_FRAMEBUFFER_BINDING, &buffer);
        GrGLFramebufferInfo info;
        info.fFBOID = (GrGLuint) buffer;

        SkColorType colorType;

        // TODO: the windowFormat is never any of these?
        auto const windowFormat = SDL_GetWindowPixelFormat(fWindow);
        int contextType;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);
        if (windowFormat == SDL_PIXELFORMAT_RGBA8888) {
            info.fFormat = GR_GL_RGBA8;
            colorType = kRGBA_8888_SkColorType;
        } else {
            colorType = kBGRA_8888_SkColorType;
            if (contextType == SDL_GL_CONTEXT_PROFILE_ES) {
                info.fFormat = GR_GL_BGRA8;
            } else {
                // We assume the internal format is RGBA8 on desktop GL
                info.fFormat = GR_GL_RGBA8;
            }
        }
        int w, h;
        SDL_GetWindowSizeInPixels(fWindow, &w, &h);

        constexpr bool createProtectedNativeBackend = false;
        info.fProtected = skgpu::Protected(createProtectedNativeBackend);
        auto target = GrBackendRenderTargets::MakeGL(w,
                                                     h,
                                                     msaaSampleCount,
                                                     stencilBits,
                                                     info);

        // setup SkSurface
        // To use distance field text, use commented out SkSurfaceProps instead
        // SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
        //                      SkSurfaceProps::kUnknown_SkPixelGeometry);
        SkSurfaceProps props;
        fSurface = SkSurfaces::WrapBackendRenderTarget(fContext.get(),
                                                       target,
                                                       kBottomLeft_GrSurfaceOrigin,
                                                       colorType,
                                                       SkColorSpace::MakeSRGB(),
                                                       &props);
    }

    return fSurface;
}

void App::destroyContext() {
    fSurface.reset(nullptr);

    if (fContext != nullptr) {
        fContext->abandonContext();
        fContext.reset(nullptr);
    }

    fContext.reset(nullptr);
}
void App::render() {

}