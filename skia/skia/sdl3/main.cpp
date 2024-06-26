#include <cstdio>
#include <cstdint>
#include <chrono>
#include <cstring>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include "include/core/SkGraphics.h"
#include "include/ports/SkFontMgr_data.h"
#include "include/core/SkSpan.h"
#include "include/svg/SkSVGCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/encode/SkPngEncoder.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkSurface.h"
#include "include/gpu/gl/GrGLInterface.h"

//#include "tools/trace/SkPerfettoTrace.h"
#include "tools/trace/ChromeTracingTracer.h"
#include "tools/trace/SkDebugfTracer.h"
#include "src/core/SkATrace.h"
#include "../skiaTracyTracer.h"

#include "tracy/Tracy.hpp"

#include "../src/render.h"

#include "imgui_internal.h"
#include "marshalling/receive.h"
#include "marshalling/send.h"
#include "buildinfo.gen.h"

#include "vectorCmdSkiaRenderer.h"
#include "marshalling/receive.h"
#include "marshalling/send.h"
#include "cliOptions.h"
#include "setupUI.h"

// This example doesn't compile with Emscripten yet! Awaiting SDL3 support.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

SkPaint fFontPaint;
VectorCmdSkiaRenderer fVectorCmdSkiaRenderer{};
size_t fTotalVectorCmdSerializedSize{};
size_t fSkpBytesWritten;
size_t fSvgBytesWritten;
size_t fPngBytesWritten;
ImZeroSkiaSetupUI fImZeroSkiaSetupUi{};
bool fFffiInterpreter;
SkColor fBackgroundColor;
bool fUseVectorCmd;

template <typename T>
static inline void applyFlag(int &flag,T val,bool v) {
    if(v) {
        flag |= val;
    } else {
        flag &= ~(val);
    }
}
static void drawImGuiVectorCmdsFB(SkCanvas &canvas) { ZoneScoped;
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

static void paint(SkSurface* surface, int width, int height) { ZoneScoped;
    ImGui::useVectorCmd = fUseVectorCmd && surface != nullptr;
    auto renderMode = fVectorCmdSkiaRenderer.getRenderMode();
    resetReceiveStat();
    resetSendStat();
    if(fFffiInterpreter) { ZoneScopedN("render fffi commands");
        render_render();
    } else { ZoneScopedN("demo window");
        ImGui::ShowDemoWindow();
    }

    SaveFormatE saveFormat = SaveFormatE_None;
    if(ImGui::Begin("ImZeroSkia Settings")) { ZoneScoped;
        fImZeroSkiaSetupUi.render(saveFormat, fVectorCmdSkiaRenderer, fUseVectorCmd,
                                  fTotalVectorCmdSerializedSize, totalSentBytes+totalReceivedBytes,
                                  fSkpBytesWritten, fSvgBytesWritten, fPngBytesWritten,
                                  width, height
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
            case SaveFormatE_SVGNoFont: { ZoneScoped;
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
                    case SaveFormatE_SVGNoFont:
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

int main(int argc, char** argv) {
#ifdef TRACY_ENABLE
    ImGui::SetAllocatorFunctions(imZeroMemAlloc,imZeroMemFree,nullptr);
#endif

    CliOptions opts{};
    VectorCmdSkiaRenderer vectorCmdSkiaRenderer{};

    opts.parse(argc, argv, stderr);
    { // setup skia/imgui shared objects
        if (opts.fffiInterpreter) {
            if (opts.fffiInFile != nullptr) {
                fdIn = fopen(opts.fffiInFile, "r");
                if (fdIn == nullptr) {
                    fprintf(stderr, "unable to open fffInFile %s: %s", opts.fffiInFile, strerror(errno));
                    exit(1);
                }
                setvbuf(fdIn, nullptr, _IONBF, 0);
            }
            if (opts.fffiOutFile != nullptr) {
                fdOut = fopen(opts.fffiOutFile, "w");
                if (fdOut == nullptr) {
                    fprintf(stderr, "unable to open fffOutFile %s: %s", opts.fffiOutFile, strerror(errno));
                    exit(1);
                }
                setvbuf(fdOut, nullptr, _IONBF, 0);
            }
        }

        if(opts.backgroundColorRGBA != nullptr && strlen(opts.backgroundColorRGBA) == 8) {
            auto const n = static_cast<uint32_t>(strtoul(opts.backgroundColorRGBA,nullptr,16));
            uint8_t a = n & 0xff;
            uint8_t b = (n >> 8) & 0xff;
            uint8_t g = (n >> 16) & 0xff;
            uint8_t r = (n >> 24) & 0xff;
            fBackgroundColor = SkColorSetARGB(a,r,g,b);
        }

        fUseVectorCmd = opts.vectorCmd;

        auto ttfData = SkData::MakeFromFileName(opts.ttfFilePath);
        auto fontMgr = SkFontMgr_New_Custom_Data(SkSpan<sk_sp<SkData>>(&ttfData, 1));
        auto const typeface = fontMgr->makeFromData(ttfData);
        //auto const typeface = fontMgr->matchFamilyStyle(nullptr,SkFontStyle());
        if(typeface == nullptr || fontMgr->countFamilies() <= 0) {
            fprintf(stderr, "unable to initialize font manager with supplied ttf font file %s\n",opts.ttfFilePath);
            exit(1);
        }

        ImGui::skiaFontDyFudge = opts.fontDyFudge;
        ImGui::paragraph = std::make_shared<Paragraph>(fontMgr, typeface);
        ImGui::skiaFont = SkFont(typeface);

        if(opts.fffiInterpreter) {
            render_init();
        }

        vectorCmdSkiaRenderer.setVertexDrawPaint(&fFontPaint);
        vectorCmdSkiaRenderer.setParagraphHandler(ImGui::paragraph);
        RenderModeE mode = 0;
        if(opts.backdropFilter) {
            mode |= RenderModeE_BackdropBlur;
        }
        if(opts.sketchFilter) {
            mode |= RenderModeE_Sketch;
        }
        vectorCmdSkiaRenderer.changeRenderMode(mode);
    }

    SDL_Window *window = nullptr;
    SDL_GLContext gl_context = nullptr;
    const char *glsl_version = nullptr;
    {
        // Setup SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0) {
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
        glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #endif

        // Enable native IME.
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
        window = SDL_CreateWindow(opts.appTitle, 1280, 720, window_flags);
        if (window == nullptr) {
            fprintf(stderr, "Error: SDL_CreateWindow(): %s\n", SDL_GetError());
            exit(1);
        }
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        gl_context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, gl_context);
        SDL_GL_SetSwapInterval(1); // Enable vsync
        SDL_ShowWindow(window);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImVec4 clear_color;
    ImGuiIO &io = ImGui::GetIO();
    {
        applyFlag(io.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard, opts.imguiNavKeyboard);
        applyFlag(io.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad, opts.imguiNavGamepad);
        applyFlag(io.ConfigFlags, ImGuiConfigFlags_DockingEnable, opts.imguiDocking);

        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // FIXME remove ?

        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);

        clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    }

    // Main loop
    bool done = false;
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!done)
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        {
            auto const width = static_cast<int>(io.DisplaySize.x);
            auto const height = static_cast<int>(io.DisplaySize.y);
#if 1
            const auto s = SkISize::Make(width, height);
            const auto c = SkColorInfo(kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
            sk_sp<SkSurface> rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(s, c));

            paint(rasterSurface.get(),width,height); // will call ImGui::Render();

            sk_sp<SkImage> image = rasterSurface->makeImageSnapshot();
#else
            if(ImGui::Begin("Hello, world!")) {
                ImGui::TextUnformatted("uh, it works!");
            }
            ImGui::End();

            ImGui::Render();
#endif

            glViewport(0, 0, width, height);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
            //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
                SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
            }

            SDL_GL_SwapWindow(window);
        }
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    if(opts.fffiInterpreter) {
        render_cleanup();
    }
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#if 0
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkSurface.h"

#include "include/gpu/gl/GrGLInterface.h"
#include "src/gpu/gl/GrGLUtil.h"

static int main2(int argc, char** argv) {
    uint32_t windowFlags = 0;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GLContext glContext = nullptr;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    static const int kStencilBits = 8;  // Skia needs 8 stencil bits
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, kStencilBits);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // If you want multisampling, uncomment the below lines and set a sample count
    static const int kMsaaSampleCount = 0; //4;
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, kMsaaSampleCount);

    /*
     * In a real application you might want to initialize more subsystems
     */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        return 1;
    }

    // Setup window
    // This code will create a window with the same resolution as the user's desktop.
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, dm.w, dm.h, windowFlags);

    if (!window) {
        handle_error();
        return 1;
    }

    // To go fullscreen
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // try and setup a GL context
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        return 1;
    }

    int success =  SDL_GL_MakeCurrent(window, glContext);
    if (success != 0) {
        return success;
    }

    uint32_t windowFormat = SDL_GetWindowPixelFormat(window);
    int contextType;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);


    int dw, dh;
    SDL_GL_GetDrawableSize(window, &dw, &dh);

    glViewport(0, 0, dw, dh);
    glClearColor(1, 1, 1, 1);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // setup GrContext
    auto interface = GrGLMakeNativeInterface();

    // setup contexts
    sk_sp<GrDirectContext> grContext(GrDirectContext::MakeGL(interface));
    SkASSERT(grContext);

    // Wrap the frame buffer object attached to the screen in a Skia render target so Skia can
    // render to it
    GrGLint buffer;
    GR_GL_GetIntegerv(interface.get(), GR_GL_FRAMEBUFFER_BINDING, &buffer);
    GrGLFramebufferInfo info;
    info.fFBOID = (GrGLuint) buffer;
    SkColorType colorType;

    //SkDebugf("%s", SDL_GetPixelFormatName(windowFormat));
    // TODO: the windowFormat is never any of these?
    if (SDL_PIXELFORMAT_RGBA8888 == windowFormat) {
        info.fFormat = GR_GL_RGBA8;
        colorType = kRGBA_8888_SkColorType;
    } else {
        colorType = kBGRA_8888_SkColorType;
        if (SDL_GL_CONTEXT_PROFILE_ES == contextType) {
            info.fFormat = GR_GL_BGRA8;
        } else {
            // We assume the internal format is RGBA8 on desktop GL
            info.fFormat = GR_GL_RGBA8;
        }
    }

    GrBackendRenderTarget target(dw, dh, kMsaaSampleCount, kStencilBits, info);

    // setup SkSurface
    // To use distance field text, use commented out SkSurfaceProps instead
    // SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
    //                      SkSurfaceProps::kUnknown_SkPixelGeometry);
    SkSurfaceProps props;

    sk_sp<SkSurface> surface(SkSurface::MakeFromBackendRenderTarget(grContext.get(), target,
                                                                    kBottomLeft_GrSurfaceOrigin,
                                                                    colorType, nullptr, &props));

    SkCanvas* canvas = surface->getCanvas();
    canvas->scale((float)dw/dm.w, (float)dh/dm.h);

    SkPaint paint;

    int rotation = 0;
    SkFont font;
    while (!state.fQuit) { // Our application loop
        canvas->clear(SK_ColorWHITE);

        paint.setColor(SK_ColorBLACK);

        canvas->flush();
        SDL_GL_SwapWindow(window);
    }

    if (glContext) {
        SDL_GL_DeleteContext(glContext);
    }

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();
    return 0;
}
#endif