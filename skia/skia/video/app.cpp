#include "app.h"

#include <cstdio>
#include <cstring>
#include <fcntl.h>

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

#include "tracy/Tracy.hpp"

#include "../src/render.h"

#include "imgui_internal.h"
#include "marshalling/receive.h"
#include "marshalling/send.h"

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
#include "qoi.h"

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
    sk_sp<SkFontMgr> fontMgr = nullptr;
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
        fontMgr = SkFontMgr_New_Custom_Data(SkSpan(&ttfData,1));
        typeface = fontMgr->makeFromData(ttfData);
        ////auto const typeface = fontMgr->matchFamilyStyle(nullptr,SkFontStyle());
        if(typeface == nullptr || fontMgr->countFamilies() <= 0) {
            fprintf(stderr, "unable to initialize font manager with supplied ttf font file %s\n",opts.ttfFilePath);
            return(1);
        }

        ImGui::skiaFontDyFudge = opts.fontDyFudge;
        ImGui::paragraph = std::make_shared<Paragraph>(fontMgr, typeface);
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
        auto userInteractionFd = open(opts.videoUserInteractionEventsInFile, O_RDONLY | O_NONBLOCK);
        if(userInteractionFd == -1) {
            fprintf(stderr, "unable to open user interaction events in file %s: %s\n", opts.videoUserInteractionEventsInFile, strerror(errno));
            return 1;
        }
        fUserInteractionFH = fdopen(userInteractionFd,"rb");
        //fUserInteractionFH = fopen(opts.videoUserInteractionEventsInFile, "rb");
        if(fUserInteractionFH == nullptr) {
            fprintf(stderr, "unable to open user interaction events in file %s: %s\n", opts.videoUserInteractionEventsInFile, strerror(errno));
            return 1;
        }

        auto const fd = fileno(fUserInteractionFH);
        auto const flags = fcntl(fd, F_GETFL);
        if(flags < 0) {
            fprintf(stderr, "unable to get file status flags for file %s: %s\n", opts.videoUserInteractionEventsInFile, strerror(errno));
            return 1;
        }
        if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
            fprintf(stderr, "unable to set file status flag O_NONBLOCK file %s: %s\n", opts.videoUserInteractionEventsInFile, strerror(errno));
            return 1;
        }
        //if(setvbuf(fUserInteractionFH,nullptr,_IONBF,0) != 0) {
        //    fprintf(stderr, "unable to set buffering for file %s: %s\n", opts.videoUserInteractionEventsInFile, strerror(errno));
        //    return 1;
        //}
    }

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
    double currentTime = SkTime::GetSecs();
    io.DeltaTime = static_cast<float>(currentTime - fPreviousTime);
    fPreviousTime = currentTime;
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
    auto stream = SkFILEWStream(opts.videoRawFramesFile);

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
                if (!SkWebpEncoder::Encode(static_cast<SkWStream *>(&stream), pixmap, webPOptions)) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
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
    auto stream = SkFILEWStream(opts.videoRawFramesFile);

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
                if (!SkJpegEncoder::Encode(static_cast<SkWStream *>(&stream), pixmap, jpegOptions)) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
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
    auto stream = SkFILEWStream(opts.videoRawFramesFile);

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
                if (!SkPngEncoder::Encode(static_cast<SkWStream *>(&stream), pixmap, pngOptions)) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
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
            if(img->peekPixels(&pixmap)) {
                if (!bmpEncoder.encode(static_cast<SkWStream *>(&stream), pixmap.addr32())) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
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
    if(fUserInteractionFH != nullptr) {
        fclose(fUserInteractionFH);
        fUserInteractionFH = nullptr;
    }
}

void App::dispatchUserInteractionEvents() {
    static size_t memorySize = 1024 * 1024;
    static uint8_t state = 0;
    static uint8_t* mem = nullptr;
    static uint8_t* p = nullptr;
    static uint32_t bytesToRead = 0;
    constexpr const int sizeOfLengthPrefix = 4;

    if(fUserInteractionFH == nullptr) {
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
                auto r = fread(p,1,bytesToRead,fUserInteractionFH);
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
                auto r = fread(p,1,bytesToRead,fUserInteractionFH);
                bytesToRead -= r;
                p += r;
                if(bytesToRead == 0) {
                    auto verifier = flatbuffers::Verifier(mem+sizeOfLengthPrefix,bytesToRead);
                    if(!UserInteractionFB::VerifyEventBuffer(verifier)) {
                        auto txt = flatbuffers::FlatBufferToString(mem+sizeOfLengthPrefix,UserInteractionFB::EventTypeTable());
                        fprintf(stderr, "userInteractionEvent=%s\n", txt.c_str());

                        auto const e = UserInteractionFB::GetSizePrefixedEvent(mem);
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

void App::handleUserInteractionEvent(UserInteractionFB::Event const &ev) {
    ImGuiIO& io = ImGui::GetIO();
    fprintf(stderr, "handling user interaction event %s\n",EnumNameUserInteraction(ev.event_type()));
    switch(ev.event_type()) {
        case UserInteractionFB::UserInteraction_NONE:
            break;
        case UserInteractionFB::UserInteraction_EventMouseMotion:
        {
            auto const e = ev.event_as_EventMouseMotion();
            io.AddMouseSourceEvent(e->is_touch() ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
            auto const p = e->pos();
            io.AddMousePosEvent(p->x(), p->y());
        }
        break;
        case UserInteractionFB::UserInteraction_EventMouseWheel:
        {
            auto const e = ev.event_as_EventMouseMotion();
            io.AddMouseSourceEvent(e->is_touch() ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
            auto const p = e->pos();
            io.AddMouseWheelEvent(-p->x(), p->y());
        }
        break;
        case UserInteractionFB::UserInteraction_EventMouseButton:
        {
            auto const e = ev.event_as_EventMouseButton();
            int mb = -1;
            auto const b = e->button();
            switch(b) {
                case UserInteractionFB::MouseButton_None: break;
                case UserInteractionFB::MouseButton_Left: mb = 0; break;
                case UserInteractionFB::MouseButton_Right: mb = 1; break;
                case UserInteractionFB::MouseButton_Middle: mb = 2; break;
                case UserInteractionFB::MouseButton_X1: mb = 3; break;
                case UserInteractionFB::MouseButton_X2: mb = 4; break;
            }
            if (mb >= 0) {
                const bool d = e->type() == UserInteractionFB::MouseButtonEventType_Down;
                io.AddMouseSourceEvent(e->is_touch() ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
                io.AddMouseButtonEvent(mb, d);
                //bd->MouseButtonsDown = d ? (bd->MouseButtonsDown | (1 << mb)) : (bd->MouseButtonsDown & ~(1 << mb));
            }
        }
        break;
        case UserInteractionFB::UserInteraction_EventTextInput:
        {
            auto const e = ev.event_as_EventTextInput();
            auto const t = e->text();

            io.AddInputCharactersUTF8(e->text()->c_str());
        }
        break;
    }
}
