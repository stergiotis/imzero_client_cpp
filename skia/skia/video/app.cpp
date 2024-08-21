#include "app.h"

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <csignal>
#include <SDL3/SDL.h>

#include "imgui.h"

#include "include/core/SkGraphics.h"
#include "include/ports/SkFontMgr_data.h"
#include "include/core/SkSpan.h"
#include "include/svg/SkSVGCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/encode/SkPngEncoder.h"
#include "include/encode/SkJpegEncoder.h"
#include "include/encode/SkWebpEncoder.h"
#include "include/gpu/GrDirectContext.h"
#include "bmpEncoder.h"
#if defined(SK_FONTMGR_FONTCONFIG_AVAILABLE)
#include "include/ports/SkFontMgr_fontconfig.h"
#endif
#if defined(SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE)
#include "include/ports/SkFontMgr_directory.h"
#endif

#include "tracy/Tracy.hpp"

#include "../src/render.h"

#include "imgui_internal.h"
#include "marshalling/receive.h"
#include "marshalling/send.h"

#include "ImZeroFB.out.h"
#include "flatbuffers/minireflect.h"

#define QOI_IMPLEMENTATION
#define QOI_NO_STDIO
#define QOI_FREE static_assert(false && "free should never be called")
#define QOI_MALLOC(sz) qoiMalloc(sz)
static void *qoiMalloc(size_t sz) {
    static void *qoiBuffer = nullptr;
    static size_t lastSz = 0;
    if(qoiBuffer == nullptr) {
        lastSz = sz;
        qoiBuffer = malloc(sz);
    }
    assert(lastSz == sz && "assuming constant width, height and depth of qoi images");
    return qoiBuffer;
}
#include "../contrib/qoi/qoi.h"

template <typename T>
static inline void applyFlag(int &flag,T val,bool v) {
    if(v) {
        flag |= val;
    } else {
        flag &= ~(val);
    }
}
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

void App::paint(SkCanvas* canvas, int width, int height) { ZoneScoped;
    ImGui::NewFrame();
    ImGui::ShowMetricsWindow();

    ImGui::useVectorCmd = fUseVectorCmd;
    resetReceiveStat();
    resetSendStat();
    if(fFffiInterpreter) { ZoneScopedN("render fffi commands");
        render_render();
    } else { ZoneScopedN("demo window");
        ImGui::ShowDemoWindow();
    }

    SaveFormatE saveFormat = SaveFormatE_Disabled;
    if(ImGui::Begin("ImZeroSkia Settings")) { ZoneScoped;
        fImZeroSkiaSetupUi.render(saveFormat, fVectorCmdSkiaRenderer, fUseVectorCmd,
                                  fTotalVectorCmdSerializedSize, totalSentBytes+totalReceivedBytes,
                                  fSkpBytesWritten, fSvgBytesWritten, fPngBytesWritten,
                                  width, height
        );
    }
    ImGui::End();
    ImGui::Render();

    if(canvas != nullptr) { ZoneScoped;
        canvas->clear(fBackgroundColor);

        canvas->save();
        drawImGuiVectorCmdsFB(*canvas);
        canvas->restore();
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
    atlas.TexID = &fontPaint;
}

static rawFrameOutputFormat resolveRawFrameOutputFormat(const char *name) {
    if(strcmp(name, "qoi") == 0) {
        return kRawFrameOutputFormat_Qoi;
    }
    if(strcmp(name, "webp_lossless") == 0) {
        return kRawFrameOutputFormat_WebP_Lossless;
    }
    if(strcmp(name, "webp_lossy") == 0) {
        return kRawFrameOutputFormat_WebP_Lossy;
    }
    if(strcmp(name, "jpeg") == 0) {
        return kRawFrameOutputFormat_Jpeg;
    }
    if(strcmp(name, "bmp_bgra8888") == 0) {
        return kRawFrameOutputFormat_Bmp_Bgra8888;
    }
    if(strcmp(name, "flatbuffers") == 0) {
        return kRawFrameOutputFormat_Flatbuffers;
    }
    if(strcmp(name, "jpeg") == 0) {
        return kRawFrameOutputFormat_Jpeg;
    }
    if(strcmp(name, "png") == 0) {
        return kRawFrameOutputFormat_Png;
    }
    if(strcmp(name, "skp") == 0) {
        return kRawFrameOutputFormat_Skp;
    }
    if(strcmp(name, "svg") == 0) {
        return kRawFrameOutputFormat_Svg;
    }
    if(strcmp(name, "svg_textaspath") == 0) {
        return kRawFrameOutputFormat_Svg_TextAsPath;
    }
    fprintf(stderr, "unhandled output format %s. disabling output.\n", name);
    return kRawFrameOutputFormat_None;
}

int App::run(CliOptions &opts) {
    // prevent SIGPIPE when writing frames or reading user interaction events
    signal(SIGPIPE, SIG_IGN);

    sk_sp<SkTypeface> typeface = nullptr;
    sk_sp<SkData> ttfData = nullptr;
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

        {
            ttfData = SkData::MakeFromFileName(opts.ttfFilePath);
            if(ttfData == nullptr || ttfData->isEmpty()) {
                fprintf(stderr, "unable to open ttf file %s\n",opts.ttfFilePath);
                exit(1);
            }
        }
        {
            fFontMgr = nullptr;

            if(opts.fontManager != nullptr && strcmp(opts.fontManager,"fontconfig") == 0) {
#if defined(SK_FONTMGR_FONTCONFIG_AVAILABLE)
                fFontMgr = SkFontMgr_New_FontConfig(nullptr);
#else
                fprintf(stderr,"SK_FONTMGR_FONTCONFIG_AVAILABLE is not defined, font manager %s not supported\n",opts.fontManager);
#endif
            }
            if(opts.fontManager != nullptr && strcmp(opts.fontManager,"directory") == 0) {
#if defined(SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE)
                fFontMgr = SkFontMgr_New_Custom_Directory(opts.fontManagerArg);
#else
                fprintf(stderr,"SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE is not defined, font manager %s not supported\n",opts.fontManager);
#endif
            }
            if(fFontMgr == nullptr) {
                // fallback
                fprintf(stderr,"using fallback font manager (opt=%s,arg=%s)\n",opts.fontManager,opts.fontManagerArg);
                fFontMgr = SkFontMgr_New_Custom_Data(SkSpan(&ttfData, 1));
            }
        }
        typeface = fFontMgr->makeFromData(ttfData);
        ////auto const typeface = fontMgr->matchFamilyStyle(nullptr,SkFontStyle());
        if(typeface == nullptr || fFontMgr->countFamilies() <= 0) {
            fprintf(stderr, "unable to initialize font manager with supplied ttf font file %s\n",opts.ttfFilePath);
            return(1);
        }

        ImGui::skiaFontDyFudge = opts.fontDyFudge;
        ImGui::paragraph = std::make_shared<Paragraph>(fFontMgr, typeface);
        ImGui::skiaFont = SkFont(typeface);

        fVectorCmdSkiaRenderer.setVertexDrawPaint(&fFontPaint);
        fVectorCmdSkiaRenderer.setParagraphHandler(ImGui::paragraph);
        RenderModeE mode = 0;
        if(opts.backdropFilter) {
            mode |= RenderModeE_BackdropBlur;
        }
        if(opts.sketchFilter) {
            mode |= RenderModeE_Sketch;
        }
        fVectorCmdSkiaRenderer.changeRenderMode(mode);
    }

    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);
    const auto wf = static_cast<float>(w);
    const auto hf = static_cast<float>(h);
    if(w == 0 || h == 0) {
        fprintf(stderr, "invalid video resolution: %ux%u\n", w,h);
        exit(1);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImVec4 clearColorImVec4;
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

        clearColorImVec4 = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        io.DisplaySize.x = wf;
        io.DisplaySize.y = hf;

        io.ConfigInputTrickleEventQueue = true;
        io.ConfigWindowsMoveFromTitleBarOnly = false; // make config option
        io.MouseDrawCursor = true;
    }

    if(opts.fffiInterpreter) {
        render_init();
    }
    build_ImFontAtlas(*io.Fonts,fFontPaint);

    fOutputFormat = resolveRawFrameOutputFormat(opts.videoRawOutputFormat);

    { // rescale font
        auto f = ImGui::GetIO().FontDefault;
        if(f != nullptr && f->IsLoaded()) {
            ImFontAtlas& atlas = *ImGui::GetIO().Fonts;
            atlas.Clear();
            ImFontConfig cfg;
            cfg.SizePixels = 13;
            atlas.AddFontDefault(&cfg);
            build_ImFontAtlas(atlas, fFontPaint);
        }
    }
    fBackgroundColor = SkColorSetARGB(clearColorImVec4.w * 255.0f, clearColorImVec4.x * 255.0f, clearColorImVec4.y * 255.0f, clearColorImVec4.z * 255.0f);

    if(opts.videoUserInteractionEventsInFile != nullptr && opts.videoUserInteractionEventsInFile[0] != '\0') {
        // RDWR: having at least one writer will prevent SIG_PIPE
        fUserInteractionFd = open(opts.videoUserInteractionEventsInFile, O_RDWR | O_NONBLOCK);
        if(fUserInteractionFd == -1) {
            fprintf(stderr, "unable to open user interaction events in file %s: %s\n", opts.videoUserInteractionEventsInFile, strerror(errno));
            return 1;
        }
        fDispatchInteractionEvents = true;
    }
    fRawFrameFileOpened = false;

    switch(fOutputFormat) {
        case kRawFrameOutputFormat_None:
            loopEmpty(opts);
            break;
        case kRawFrameOutputFormat_WebP_Lossless: // fallthrough
        case kRawFrameOutputFormat_WebP_Lossy:
            loopWebp(opts);
            break;
        case kRawFrameOutputFormat_Jpeg:
            loopJpeg(opts);
            break;
        case kRawFrameOutputFormat_Png:
            loopPng(opts);
            break;
        case kRawFrameOutputFormat_Qoi:
            loopQoi(opts);
            break;
        case kRawFrameOutputFormat_Bmp_Bgra8888:
            loopBmp(opts);
            break;
        case kRawFrameOutputFormat_Flatbuffers:
            loopFlatbuffers(opts);
            break;
        case kRawFrameOutputFormat_Svg: // fallthrough
        case kRawFrameOutputFormat_Svg_TextAsPath:
            loopSvg(opts);
            break;
        case kRawFrameOutputFormat_Skp:
            loopSkp(opts);
            break;
    }

    if(opts.fffiInterpreter) {
        render_cleanup();
    }
    ImGui::DestroyContext();

    return 0;
}
void App::postPaint() {
    ImGuiIO& io = ImGui::GetIO();

    static uint64_t frequency = SDL_GetPerformanceFrequency();
    uint64_t current_time = SDL_GetPerformanceCounter();
    if(current_time <= fTime) {
        current_time = fTime + 1;
    }
    io.DeltaTime = fTime > 0 ? (float)((double)(current_time - fTime) / static_cast<double>(frequency)) : (float)(1.0f / 60.0f);
    fTime = current_time;

    fFrame++;
    dispatchUserInteractionEvents();
}

App::App() {
    fTotalVectorCmdSerializedSize = 0;
    fSkpBytesWritten = 0;
    fSvgBytesWritten = 0;
    fPngBytesWritten = 0;
    fBackgroundColor = SK_ColorRED;
    fFffiInterpreter = false;
    fUseVectorCmd = false;
    fFrame = 0;
    fPreviousTime = 0.0;
}

void App::loopEmpty(const CliOptions &opts) {
    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);
    uint64_t maxFrame = opts.videoExitAfterNFrames;

    while(maxFrame == 0 || fFrame < maxFrame) {
        paint(nullptr,w,h);
        postPaint();
    }
}

void App::loopWebp(const CliOptions &opts) {
    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);
    SkWebpEncoder::Options webPOptions;

    SkColorType colorType = kRGBA_8888_SkColorType;
    SkAlphaType alphaType = kPremul_SkAlphaType;
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    const auto c = SkColorInfo(colorType, alphaType, colorSpace);
    const auto rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(SkISize::Make(w,h), c));
    auto canvas = rasterSurface->getCanvas();

    switch(fOutputFormat) {
        case kRawFrameOutputFormat_WebP_Lossy:
            webPOptions.fCompression = SkWebpEncoder::Compression::kLossy;
            webPOptions.fQuality = 70.0f;
            break;
        case kRawFrameOutputFormat_WebP_Lossless:
            webPOptions.fCompression = SkWebpEncoder::Compression::kLossless;
            webPOptions.fQuality = 0.0f;
            break;
    }
    uint64_t maxFrame = opts.videoExitAfterNFrames;

    paint(canvas,w,h);
    postPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        paint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if(img->peekPixels(&pixmap)) {
                ensureRawFrameFileOpened(opts);
                if (!SkWebpEncoder::Encode(static_cast<SkWStream *>(fRawFrameFileStream.get()), pixmap, webPOptions)) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                    fRawFrameFileOpened = false;
                }
            }
        }
        postPaint();
    }
}

void App::loopJpeg(const CliOptions &opts) {
    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);
    SkJpegEncoder::Options jpegOptions;

    SkColorType colorType = kRGBA_8888_SkColorType;
    SkAlphaType alphaType = kPremul_SkAlphaType;
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    const auto c = SkColorInfo(colorType, alphaType, colorSpace);
    const auto rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(SkISize::Make(w,h), c));
    auto canvas = rasterSurface->getCanvas();

    jpegOptions.fQuality = 80;
    uint64_t maxFrame = opts.videoExitAfterNFrames;

    paint(canvas,w,h);
    postPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        paint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if(img->peekPixels(&pixmap)) {
                ensureRawFrameFileOpened(opts);
                if (!SkJpegEncoder::Encode(static_cast<SkWStream *>(fRawFrameFileStream.get()), pixmap, jpegOptions)) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                    fRawFrameFileOpened = false;
                }
            }
        }
        postPaint();
    }
}

void App::loopPng(const CliOptions &opts) {
    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);
    SkPngEncoder::Options pngOptions;

    SkColorType colorType = kRGBA_8888_SkColorType;
    SkAlphaType alphaType = kPremul_SkAlphaType;
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    const auto c = SkColorInfo(colorType, alphaType, colorSpace);
    const auto rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(SkISize::Make(w,h), c));
    auto canvas = rasterSurface->getCanvas();

    pngOptions.fZLibLevel = 6;
    uint64_t maxFrame = opts.videoExitAfterNFrames;

    paint(canvas,w,h);
    postPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        paint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if(img->peekPixels(&pixmap)) {
                ensureRawFrameFileOpened(opts);
                if (!SkPngEncoder::Encode(static_cast<SkWStream *>(fRawFrameFileStream.get()), pixmap, pngOptions)) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                    fRawFrameFileOpened = false;
                }
            }
        }
        postPaint();
    }
}

void App::loopQoi(const CliOptions &opts) {
    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);
    qoi_desc qd{
            static_cast<unsigned int>(w),
            static_cast<unsigned int>(h),
            4,
            QOI_SRGB
    };

    SkColorType colorType = kRGBA_8888_SkColorType;
    SkAlphaType alphaType = kPremul_SkAlphaType;
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    const auto c = SkColorInfo(colorType, alphaType, colorSpace);
    const auto rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(SkISize::Make(w,h), c));
    auto canvas = rasterSurface->getCanvas();
    auto stream = SkFILEWStream(opts.videoRawFramesFile);
    uint64_t maxFrame = opts.videoExitAfterNFrames;

    paint(canvas,w,h);
    postPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        paint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if (img->peekPixels(&pixmap)) {
                int imgLen;
                auto const imgMem = qoi_encode(pixmap.addr(), &qd, &imgLen);
                stream.write(imgMem, static_cast<size_t>(imgLen));
                stream.flush();
            }
        }
        postPaint();
    }
}

void App::loopBmp(const CliOptions &opts) {
    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);
    const auto bmpEncoder = BmpBGRA8888Encoder(w,h);

    SkColorType colorType = kBGRA_8888_SkColorType;
    SkAlphaType alphaType = kPremul_SkAlphaType;
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    const auto c = SkColorInfo(colorType, alphaType, colorSpace);
    const auto rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(SkISize::Make(w,h), c));
    auto canvas = rasterSurface->getCanvas();

    uint64_t maxFrame = opts.videoExitAfterNFrames;

    paint(canvas,w,h);
    postPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        paint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if(img->peekPixels(&pixmap)) {
                ensureRawFrameFileOpened(opts);
                if (!bmpEncoder.encode(static_cast<SkWStream *>(fRawFrameFileStream.get()), pixmap.addr32())) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                    fRawFrameFileOpened = false;
                }
            }
        }
        postPaint();
    }
}

void App::loopFlatbuffers(const CliOptions &opts) {
    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);
    auto stream = SkFILEWStream(opts.videoRawFramesFile);
    uint64_t maxFrame = opts.videoExitAfterNFrames;
    fFrame = 0;
    fVectorCmdSkiaRenderer.changeRenderMode(RenderModeE_Normal);
    fUseVectorCmd = true;
    paint(nullptr,w,h);
    postPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        fUseVectorCmd = true;
        paint(nullptr,w,h);
        { ZoneScoped;
            const ImDrawData* drawData = ImGui::GetDrawData();
            fTotalVectorCmdSerializedSize = 0;
            for (int i = 0; i < drawData->CmdListsCount; ++i) {
                ImDrawList* drawList = drawData->CmdLists[i];
                const uint8_t *buf;
                size_t sz;
                drawList->serializeFB(buf,sz);
                stream.write(buf,sz);
            }
            stream.flush();
        }
        postPaint();
    }
}

void App::loopSvg(const CliOptions &opts) {
    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);
    int svgCanvasFlags = 0;

    auto stream = SkFILEWStream(opts.videoRawFramesFile);
    SkCanvas *canvas = nullptr;

    switch(fOutputFormat) {
        case kRawFrameOutputFormat_Svg:
            svgCanvasFlags = SkSVGCanvas::kNoPrettyXML_Flag | SkSVGCanvas::kRelativePathEncoding_Flag;
            fVectorCmdSkiaRenderer.changeRenderMode(fVectorCmdSkiaRenderer.getRenderMode() | RenderModeE_SVG);
            canvas = SkSVGCanvas::Make(SkRect::MakeIWH(w,h), &stream, svgCanvasFlags).release();
            break;
        case kRawFrameOutputFormat_Svg_TextAsPath:
            svgCanvasFlags = SkSVGCanvas::kNoPrettyXML_Flag | SkSVGCanvas::kRelativePathEncoding_Flag | SkSVGCanvas::kConvertTextToPaths_Flag;
            fVectorCmdSkiaRenderer.changeRenderMode(fVectorCmdSkiaRenderer.getRenderMode() | RenderModeE_SVG);
            canvas = SkSVGCanvas::Make(SkRect::MakeIWH(w,h), &stream, svgCanvasFlags).release();
            break;
    }
    uint64_t maxFrame = opts.videoExitAfterNFrames;

    paint(canvas,w,h);
    postPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        paint(canvas,w,h);
        postPaint();
    }

    // delete SkSvgCanvas to flush commands (no necessary for other canvas implementations)
    delete canvas;
}

void App::loopSkp(const CliOptions &opts) {
    const int w = static_cast<int>(opts.videoResolutionWidth);
    const int h = static_cast<int>(opts.videoResolutionHeight);

    SkPictureRecorder skiaRecorder;
    auto stream = SkFILEWStream(opts.videoRawFramesFile);

    uint64_t maxFrame = opts.videoExitAfterNFrames;

    paint(nullptr,w,h);
    postPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        auto canvas = skiaRecorder.beginRecording(SkIntToScalar(w), SkIntToScalar(h));
        paint(canvas,w,h);

        { ZoneScoped;
            sk_sp<SkPicture> picture = skiaRecorder.finishRecordingAsPicture();
            picture->serialize(&stream);
        }
        postPaint();
    }
}
App::~App() {
    if(fDispatchInteractionEvents) {
        close(fUserInteractionFd);
        fDispatchInteractionEvents = false;
    }
}

void App::dispatchUserInteractionEvents() {
    static size_t memorySize = 1024 * 1024;
    static uint8_t state = 0;
    static uint8_t* mem = nullptr;
    static uint8_t* p = nullptr;
    static uint32_t bytesToRead = 0;
    constexpr const int sizeOfLengthPrefix = 4;

    if(!fDispatchInteractionEvents) {
        return;
    }
    while(true) {
        switch(state) {
            case 0: // init
                mem = static_cast<uint8_t *>(malloc(memorySize));
                if(mem == nullptr) {
                    fprintf(stderr,"unable to allocate memory\n");
                    exit(2);
                }
                state = 1;
                bytesToRead = sizeOfLengthPrefix;
                p = mem;
                break;
            case 1: // read flatbuffers message length
            {
                auto r = read(fUserInteractionFd,p,bytesToRead);
                if(r <= 0) {
                    return;
                }
                bytesToRead -= r;
                p += r;
                if(bytesToRead == 0) {
                    // read length of message
                    bytesToRead = flatbuffers::ReadScalar<uint32_t>(mem);
                    if(bytesToRead > memorySize) {
                        memorySize = (bytesToRead/4096+1)*4096;
                        mem = static_cast<uint8_t *>(realloc(mem, memorySize));
                        p = mem+sizeOfLengthPrefix;
                    }
                    state = 2;
                }
            }
            break;
            case 2: // read flatbuffers message
            {
                //fprintf(stderr, "reading message of size %d\n", (int)bytesToRead);
                auto r = read(fUserInteractionFd,p,bytesToRead);
                bytesToRead -= r;
                p += r;
                if(bytesToRead == 0) {
                    auto verifier = flatbuffers::Verifier(mem+sizeOfLengthPrefix,bytesToRead);
                    if(!verifier.VerifyBuffer<ImZeroFB::InputEvent>()) {
                        auto txt = flatbuffers::FlatBufferToString(mem+sizeOfLengthPrefix,ImZeroFB::InputEventTypeTable());
                        fprintf(stderr, "userInteractionEvent=%s\n", txt.c_str());

                        auto const e = flatbuffers::GetSizePrefixedRoot<ImZeroFB::InputEvent>(mem);
                        handleUserInteractionEvent(*e);
                        state = 1;
                        p = mem;
                        bytesToRead = sizeOfLengthPrefix;
                    } else {
                        fprintf(stderr, "received corrupt user interaction event!\n");
                        exit(1);
                    }
                }
            }
            break;
        }
    }
}
static ImGuiKey keyCodeToImGuiKey(ImZeroFB::KeyCode keyCode) {
    switch(keyCode) {
        case ImZeroFB::KeyCode_Key_Tab:
            return ImGuiKey_Tab;
        case ImZeroFB::KeyCode_Key_LeftArrow:
            return ImGuiKey_LeftArrow;
        case ImZeroFB::KeyCode_Key_RightArrow:
            return ImGuiKey_RightArrow;
        case ImZeroFB::KeyCode_Key_UpArrow:
            return ImGuiKey_UpArrow;
        case ImZeroFB::KeyCode_Key_DownArrow:
            return ImGuiKey_DownArrow;
        case ImZeroFB::KeyCode_Key_PageUp:
            return ImGuiKey_PageUp;
        case ImZeroFB::KeyCode_Key_PageDown:
            return ImGuiKey_PageDown;
        case ImZeroFB::KeyCode_Key_Home:
            return ImGuiKey_Home;
        case ImZeroFB::KeyCode_Key_End:
            return ImGuiKey_End;
        case ImZeroFB::KeyCode_Key_Insert:
            return ImGuiKey_Insert;
        case ImZeroFB::KeyCode_Key_Delete:
            return ImGuiKey_Delete;
        case ImZeroFB::KeyCode_Key_Backspace:
            return ImGuiKey_Backspace;
        case ImZeroFB::KeyCode_Key_Space:
            return ImGuiKey_Space;
        case ImZeroFB::KeyCode_Key_Enter:
            return ImGuiKey_Enter;
        case ImZeroFB::KeyCode_Key_Escape:
            return ImGuiKey_Escape;
        case ImZeroFB::KeyCode_Key_Apostrophe:
            return ImGuiKey_Apostrophe;
        case ImZeroFB::KeyCode_Key_Comma:
            return ImGuiKey_Comma;
        case ImZeroFB::KeyCode_Key_Minus:
            return ImGuiKey_Minus;
        case ImZeroFB::KeyCode_Key_Period:
            return ImGuiKey_Period;
        case ImZeroFB::KeyCode_Key_Slash:
            return ImGuiKey_Slash;
        case ImZeroFB::KeyCode_Key_Semicolon:
            return ImGuiKey_Semicolon;
        case ImZeroFB::KeyCode_Key_Equal:
            return ImGuiKey_Equal;
        case ImZeroFB::KeyCode_Key_LeftBracket:
            return ImGuiKey_LeftBracket;
        case ImZeroFB::KeyCode_Key_Backslash:
            return ImGuiKey_Backslash;
        case ImZeroFB::KeyCode_Key_RightBracket:
            return ImGuiKey_RightBracket;
        case ImZeroFB::KeyCode_Key_GraveAccent:
            return ImGuiKey_GraveAccent;
        case ImZeroFB::KeyCode_Key_CapsLock:
            return ImGuiKey_CapsLock;
        case ImZeroFB::KeyCode_Key_ScrollLock:
            return ImGuiKey_ScrollLock;
        case ImZeroFB::KeyCode_Key_NumLock:
            return ImGuiKey_NumLock;
        case ImZeroFB::KeyCode_Key_PrintScreen:
            return ImGuiKey_PrintScreen;
        case ImZeroFB::KeyCode_Key_Pause:
            return ImGuiKey_Pause;
        case ImZeroFB::KeyCode_Key_Keypad0:
            return ImGuiKey_Keypad0;
        case ImZeroFB::KeyCode_Key_Keypad1:
            return ImGuiKey_Keypad1;
        case ImZeroFB::KeyCode_Key_Keypad2:
            return ImGuiKey_Keypad2;
        case ImZeroFB::KeyCode_Key_Keypad3:
            return ImGuiKey_Keypad3;
        case ImZeroFB::KeyCode_Key_Keypad4:
            return ImGuiKey_Keypad4;
        case ImZeroFB::KeyCode_Key_Keypad5:
            return ImGuiKey_Keypad5;
        case ImZeroFB::KeyCode_Key_Keypad6:
            return ImGuiKey_Keypad6;
        case ImZeroFB::KeyCode_Key_Keypad7:
            return ImGuiKey_Keypad7;
        case ImZeroFB::KeyCode_Key_Keypad8:
            return ImGuiKey_Keypad8;
        case ImZeroFB::KeyCode_Key_Keypad9:
            return ImGuiKey_Keypad9;
        case ImZeroFB::KeyCode_Key_KeypadDecimal:
            return ImGuiKey_KeypadDecimal;
        case ImZeroFB::KeyCode_Key_KeypadDivide:
            return ImGuiKey_KeypadDivide;
        case ImZeroFB::KeyCode_Key_KeypadMultiply:
            return ImGuiKey_KeypadMultiply;
        case ImZeroFB::KeyCode_Key_KeypadSubtract:
            return ImGuiKey_KeypadSubtract;
        case ImZeroFB::KeyCode_Key_KeypadAdd:
            return ImGuiKey_KeypadAdd;
        case ImZeroFB::KeyCode_Key_KeypadEnter:
            return ImGuiKey_KeypadEnter;
        case ImZeroFB::KeyCode_Key_KeypadEqual:
            return ImGuiKey_KeypadEqual;
        case ImZeroFB::KeyCode_Key_LeftCtrl:
            return ImGuiKey_LeftCtrl;
        case ImZeroFB::KeyCode_Key_LeftShift:
            return ImGuiKey_LeftShift;
        case ImZeroFB::KeyCode_Key_LeftAlt:
            return ImGuiKey_LeftAlt;
        case ImZeroFB::KeyCode_Key_LeftSuper:
            return ImGuiKey_LeftSuper;
        case ImZeroFB::KeyCode_Key_RightCtrl:
            return ImGuiKey_RightCtrl;
        case ImZeroFB::KeyCode_Key_RightShift:
            return ImGuiKey_RightShift;
        case ImZeroFB::KeyCode_Key_RightAlt:
            return ImGuiKey_RightAlt;
        case ImZeroFB::KeyCode_Key_RightSuper:
            return ImGuiKey_RightSuper;
        case ImZeroFB::KeyCode_Key_Menu:
            return ImGuiKey_Menu;
        case ImZeroFB::KeyCode_Key_0:
            return ImGuiKey_0;
        case ImZeroFB::KeyCode_Key_1:
            return ImGuiKey_1;
        case ImZeroFB::KeyCode_Key_2:
            return ImGuiKey_2;
        case ImZeroFB::KeyCode_Key_3:
            return ImGuiKey_3;
        case ImZeroFB::KeyCode_Key_4:
            return ImGuiKey_4;
        case ImZeroFB::KeyCode_Key_5:
            return ImGuiKey_5;
        case ImZeroFB::KeyCode_Key_6:
            return ImGuiKey_6;
        case ImZeroFB::KeyCode_Key_7:
            return ImGuiKey_7;
        case ImZeroFB::KeyCode_Key_8:
            return ImGuiKey_8;
        case ImZeroFB::KeyCode_Key_9:
            return ImGuiKey_9;
        case ImZeroFB::KeyCode_Key_A:
            return ImGuiKey_A;
        case ImZeroFB::KeyCode_Key_B:
            return ImGuiKey_B;
        case ImZeroFB::KeyCode_Key_C:
            return ImGuiKey_C;
        case ImZeroFB::KeyCode_Key_D:
            return ImGuiKey_D;
        case ImZeroFB::KeyCode_Key_E:
            return ImGuiKey_E;
        case ImZeroFB::KeyCode_Key_F:
            return ImGuiKey_F;
        case ImZeroFB::KeyCode_Key_G:
            return ImGuiKey_G;
        case ImZeroFB::KeyCode_Key_H:
            return ImGuiKey_H;
        case ImZeroFB::KeyCode_Key_I:
            return ImGuiKey_I;
        case ImZeroFB::KeyCode_Key_J:
            return ImGuiKey_J;
        case ImZeroFB::KeyCode_Key_K:
            return ImGuiKey_K;
        case ImZeroFB::KeyCode_Key_L:
            return ImGuiKey_L;
        case ImZeroFB::KeyCode_Key_M:
            return ImGuiKey_M;
        case ImZeroFB::KeyCode_Key_N:
            return ImGuiKey_N;
        case ImZeroFB::KeyCode_Key_O:
            return ImGuiKey_O;
        case ImZeroFB::KeyCode_Key_P:
            return ImGuiKey_P;
        case ImZeroFB::KeyCode_Key_Q:
            return ImGuiKey_Q;
        case ImZeroFB::KeyCode_Key_R:
            return ImGuiKey_R;
        case ImZeroFB::KeyCode_Key_S:
            return ImGuiKey_S;
        case ImZeroFB::KeyCode_Key_T:
            return ImGuiKey_T;
        case ImZeroFB::KeyCode_Key_U:
            return ImGuiKey_U;
        case ImZeroFB::KeyCode_Key_V:
            return ImGuiKey_V;
        case ImZeroFB::KeyCode_Key_W:
            return ImGuiKey_W;
        case ImZeroFB::KeyCode_Key_X:
            return ImGuiKey_X;
        case ImZeroFB::KeyCode_Key_Y:
            return ImGuiKey_Y;
        case ImZeroFB::KeyCode_Key_Z:
            return ImGuiKey_Z;
        case ImZeroFB::KeyCode_Key_F1:
            return ImGuiKey_F1;
        case ImZeroFB::KeyCode_Key_F2:
            return ImGuiKey_F2;
        case ImZeroFB::KeyCode_Key_F3:
            return ImGuiKey_F3;
        case ImZeroFB::KeyCode_Key_F4:
            return ImGuiKey_F4;
        case ImZeroFB::KeyCode_Key_F5:
            return ImGuiKey_F5;
        case ImZeroFB::KeyCode_Key_F6:
            return ImGuiKey_F6;
        case ImZeroFB::KeyCode_Key_F7:
            return ImGuiKey_F7;
        case ImZeroFB::KeyCode_Key_F8:
            return ImGuiKey_F8;
        case ImZeroFB::KeyCode_Key_F9:
            return ImGuiKey_F9;
        case ImZeroFB::KeyCode_Key_F10:
            return ImGuiKey_F10;
        case ImZeroFB::KeyCode_Key_F11:
            return ImGuiKey_F11;
        case ImZeroFB::KeyCode_Key_F12:
            return ImGuiKey_F12;
        case ImZeroFB::KeyCode_Key_F13:
            return ImGuiKey_F13;
        case ImZeroFB::KeyCode_Key_F14:
            return ImGuiKey_F14;
        case ImZeroFB::KeyCode_Key_F15:
            return ImGuiKey_F15;
        case ImZeroFB::KeyCode_Key_F16:
            return ImGuiKey_F16;
        case ImZeroFB::KeyCode_Key_F17:
            return ImGuiKey_F17;
        case ImZeroFB::KeyCode_Key_F18:
            return ImGuiKey_F18;
        case ImZeroFB::KeyCode_Key_F19:
            return ImGuiKey_F19;
        case ImZeroFB::KeyCode_Key_F20:
            return ImGuiKey_F20;
        case ImZeroFB::KeyCode_Key_F21:
            return ImGuiKey_F21;
        case ImZeroFB::KeyCode_Key_F22:
            return ImGuiKey_F22;
        case ImZeroFB::KeyCode_Key_F23:
            return ImGuiKey_F23;
        case ImZeroFB::KeyCode_Key_F24:
            return ImGuiKey_F24;
        case ImZeroFB::KeyCode_Key_AppBack:
            return ImGuiKey_AppBack;
        case ImZeroFB::KeyCode_Key_AppForward:
            return ImGuiKey_AppForward;
        case ImZeroFB::KeyCode_Key_None: // fallthrough
        default:
            return ImGuiKey_None;
    }
}

void App::handleUserInteractionEvent(ImZeroFB::InputEvent const &ev) {
    ImGuiIO& io = ImGui::GetIO();
    //fprintf(stderr, "handling user interaction event %s\n",EnumNameUserInteraction(ev.event_type()));
    switch(ev.event_type()) {
        case ImZeroFB::UserInteraction_NONE:
            break;
        case ImZeroFB::UserInteraction_EventMouseMotion:
        {
            auto const e = ev.event_as_EventMouseMotion();
            io.AddMouseSourceEvent(e->is_touch() ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
            auto const p = e->pos();
            io.AddMousePosEvent(p->x(), p->y());
        }
        break;
        case ImZeroFB::UserInteraction_EventMouseWheel:
        {
            auto const e = ev.event_as_EventMouseMotion();
            io.AddMouseSourceEvent(e->is_touch() ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
            auto const p = e->pos();
            io.AddMouseWheelEvent(-p->x(), p->y());
        }
        break;
        case ImZeroFB::UserInteraction_EventMouseButton:
        {
            auto const e = ev.event_as_EventMouseButton();
            int mb = -1;
            auto const b = e->button();
            switch(b) {
                case ImZeroFB::MouseButton_None: break;
                case ImZeroFB::MouseButton_Left: mb = 0; break;
                case ImZeroFB::MouseButton_Right: mb = 1; break;
                case ImZeroFB::MouseButton_Middle: mb = 2; break;
                case ImZeroFB::MouseButton_X1: mb = 3; break;
                case ImZeroFB::MouseButton_X2: mb = 4; break;
            }
            if (mb >= 0) {
                const bool d = e->type() == ImZeroFB::MouseButtonEventType_Down;
                io.AddMouseSourceEvent(e->is_touch() ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
                io.AddMouseButtonEvent(mb, d);
                //bd->MouseButtonsDown = d ? (bd->MouseButtonsDown | (1 << mb)) : (bd->MouseButtonsDown & ~(1 << mb));
            }
        }
        break;
        case ImZeroFB::UserInteraction_EventKeyboard:
        {
           auto const e = ev.event_as_EventKeyboard();
           auto const keyMod = e->modifiers();
           auto const key= keyCodeToImGuiKey(e->code());
           io.AddKeyEvent(ImGuiMod_Ctrl, (keyMod & ImZeroFB::KeyModifiers_Ctrl) != 0);
           io.AddKeyEvent(ImGuiMod_Shift, (keyMod & ImZeroFB::KeyModifiers_Shift) != 0);
           io.AddKeyEvent(ImGuiMod_Alt, (keyMod & ImZeroFB::KeyModifiers_Alt) != 0);
           io.AddKeyEvent(ImGuiMod_Super, (keyMod & ImZeroFB::KeyModifiers_Super) != 0);
           io.AddKeyEvent(key, e->is_down());
           io.SetKeyEventNativeData(key,static_cast<int>(e->native_sym()),static_cast<int>(e->scancode()));
        }
        break;
        case ImZeroFB::UserInteraction_EventTextInput:
        {
            auto const e = ev.event_as_EventTextInput();
            auto const t = e->text();

            io.AddInputCharactersUTF8(e->text()->c_str());
        }
        break;
    }
}

void App::ensureRawFrameFileOpened(const CliOptions &opts) {
    if(!fRawFrameFileOpened || fRawFrameFileStream == nullptr) {
        if(fRawFrameFileStream != nullptr) {
            fprintf(stderr,"re-opening raw frames file\n");
            fRawFrameFileStream.reset();
        }
        fRawFrameFileStream = std::make_unique<SkFILEWStream>(opts.videoRawFramesFile);
        if(!fRawFrameFileStream->isValid()) {
            fprintf(stderr,"unable to open video raw frame file %s\n",opts.videoRawFramesFile);
            exit(1);
        }
        fRawFrameFileOpened = true;
    }
}
