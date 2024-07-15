#include "app.h"

#include <cstdio>
#include <cstring>

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

#define QOI_IMPLEMENTATION
#define QOI_NO_STDIO
#define QOI_FREE static_assert(false && "free should never be called")
#define QOI_MALLOC(sz) qoi_malloc(sz)
static void *qoi_malloc(size_t sz) {
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
void App::DrawImGuiVectorCmdsFB(SkCanvas &canvas) { ZoneScoped;
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

void App::Paint(SkSurface* surface, int width, int height) { ZoneScoped;
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
                DrawImGuiVectorCmdsFB(*skiaCanvas);
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
                            DrawImGuiVectorCmdsFB(*skiaCanvas);
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
                            DrawImGuiVectorCmdsFB(*skiaCanvas);
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
                DrawImGuiVectorCmdsFB(*rasterCanvas);
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
        DrawImGuiVectorCmdsFB(*skiaCanvas);
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
    atlas.TexID = &fontPaint;
}

static rawFrameOutputFormat resolveRawFrameOutputFormat(const char *name) {
    if(strcmp(name, "qoi") == 0) {
        fprintf(stderr, "using output format %s\n", name);
        return kRawFrameOutputFormat_Qoi;
    }
    if(strcmp(name, "webp_lossless") == 0) {
        fprintf(stderr, "using output format %s\n", name);
        return kRawFrameOutputFormat_WebP_Lossless;
    }
    if(strcmp(name, "webp_lossy") == 0) {
        fprintf(stderr, "using output format %s\n", name);
        return kRawFrameOutputFormat_WebP_Lossy;
    }
    if(strcmp(name, "jpeg") == 0) {
        fprintf(stderr, "using output format %s\n", name);
        return kRawFrameOutputFormat_Jpeg;
    }
    if(strcmp(name, "bmp_bgra8888") == 0) {
        fprintf(stderr, "using output format %s\n", name);
        return kRawFrameOutputFormat_Bmp_Bgra8888;
    }
    if(strcmp(name, "flatbuffers") == 0) {
        fprintf(stderr, "using output format %s\n", name);
        return kRawFrameOutputFormat_Flatbuffers;
    }
    if(strcmp(name, "pam") == 0) {
        fprintf(stderr, "using output format %s\n", name);
        return kRawFrameOutputFormat_PAM;
    }
    if(strcmp(name, "jpeg") == 0) {
        fprintf(stderr, "using output format %s\n", name);
        return kRawFrameOutputFormat_Jpeg;
    }
    if(strcmp(name, "png") == 0) {
        fprintf(stderr, "using output format %s\n", name);
        return kRawFrameOutputFormat_Png;
    }
    return kRawFrameOutputFormat_None;
}

int App::Run(CliOptions &opts) {
    sk_sp<SkFontMgr> fontMgr = nullptr;
    sk_sp<SkTypeface> typeface = nullptr;
    sk_sp<SkData> ttfData = nullptr;
    SkMemoryStream *ttfStream = nullptr;
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

    const int w = opts.videoResolutionWidth;
    const auto wf = static_cast<float>(w);
    const int h = opts.videoResolutionHeight;
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

    auto const outputFormat = resolveRawFrameOutputFormat(opts.videoRawOutputFormat);

    SkColorType colorType;
    SkAlphaType alphaType = kPremul_SkAlphaType;
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    switch(outputFormat) {
        case kRawFrameOutputFormat_Bmp_Bgra8888:
            colorType = kBGRA_8888_SkColorType;
            break;
        default:
            colorType = kRGBA_8888_SkColorType;
    }
    const auto c = SkColorInfo(colorType, alphaType, colorSpace);
    const auto rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(SkISize::Make(w,h), c));

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
    double previousTime = 0.0;

    bool done = false;
    char pathBuffer[4096];
    auto clearColor = SkColorSetARGB(clearColorImVec4.w * 255.0f, clearColorImVec4.x * 255.0f, clearColorImVec4.y * 255.0f, clearColorImVec4.z * 255.0f);

    SkWebpEncoder::Options webPOptions;
    SkJpegEncoder::Options jpegOptions;
    SkPngEncoder::Options pngOptions;
    const auto bmpEncoder = BmpBGRA8888Encoder(w,h);
    qoi_desc qd{
            static_cast<unsigned int>(w),
            static_cast<unsigned int>(h),
            4,
            QOI_SRGB
    };
    char pamHeader[4096];
    snprintf(pamHeader, sizeof(pamHeader), "P7\nWIDTH %d\nHEIGHT %d\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n", w, h);
    auto pamHeaderLen = strlen(pamHeader);
    switch(outputFormat) {
        case kRawFrameOutputFormat_WebP_Lossy:
            webPOptions.fCompression = SkWebpEncoder::Compression::kLossy;
            webPOptions.fQuality = 70.0f;
            break;
        case kRawFrameOutputFormat_WebP_Lossless:
            webPOptions.fCompression = SkWebpEncoder::Compression::kLossless;
            webPOptions.fQuality = 0.0f;
            break;
        case kRawFrameOutputFormat_None:
            break;
        case kRawFrameOutputFormat_Jpeg:
            jpegOptions.fQuality = 80;
            break;
        case kRawFrameOutputFormat_Png:
            pngOptions.fZLibLevel = 6;
            break;
        case kRawFrameOutputFormat_Qoi:
            break;
        case kRawFrameOutputFormat_Bmp_Bgra8888:
            break;
        case kRawFrameOutputFormat_PAM:
            break;
        case kRawFrameOutputFormat_Flatbuffers:
            break;
    }
    auto stream = SkFILEWStream(opts.videoRawFramesFile);
    uint64_t frame = 0;
    uint64_t maxFrame = opts.videoExitAfterNFrames;

    {
        ImGui::NewFrame();
        ImGui::ShowMetricsWindow();
        auto canvas = rasterSurface->getCanvas();
        canvas->clear(clearColor);
        Paint(rasterSurface.get(),w,h); // will call ImGui::Render();
    }
    while(maxFrame == 0 || frame < maxFrame) {
        ImGui::NewFrame();

        ImGui::ShowMetricsWindow();

        { ZoneScoped;
            auto canvas = rasterSurface->getCanvas();
            canvas->clear(clearColor);
            Paint(rasterSurface.get(),w,h); // will call ImGui::Render();
        }

        sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));

        switch(outputFormat) {
            case kRawFrameOutputFormat_WebP_Lossless: // fallthrough
            case kRawFrameOutputFormat_WebP_Lossy:
                { ZoneScoped;
                    SkPixmap pixmap;
                    if(img->peekPixels(&pixmap)) {
                        if (!SkWebpEncoder::Encode(static_cast<SkWStream *>(&stream), pixmap, webPOptions)) {
                            fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                        }
                    }
                }
                break;
            case kRawFrameOutputFormat_Qoi:
                { ZoneScoped;
                    SkPixmap pixmap;
                    if (img->peekPixels(&pixmap)) {
                        int imgLen;
                        auto const imgMem = qoi_encode(pixmap.addr(), &qd, &imgLen);
                        stream.write(imgMem, static_cast<size_t>(imgLen));
                        stream.flush();
                    }
                }
                break;
            case kRawFrameOutputFormat_PAM:
                { ZoneScoped;
                    SkPixmap pixmap;
                    if (img->peekPixels(&pixmap)) {
                        stream.write(pamHeader, pamHeaderLen);
                        stream.write(pixmap.addr(),pixmap.computeByteSize());
                        stream.flush();
                    }
                }
                break;
            case kRawFrameOutputFormat_Flatbuffers:
                if(false){ ZoneScoped;
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
                break;
            case kRawFrameOutputFormat_None:
                break;
            case kRawFrameOutputFormat_Jpeg:
                { ZoneScoped;
                    SkPixmap pixmap;
                    if(img->peekPixels(&pixmap)) {
                        if (!SkJpegEncoder::Encode(static_cast<SkWStream *>(&stream), pixmap, jpegOptions)) {
                            fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                        }
                    }
                }
                break;
            case kRawFrameOutputFormat_Bmp_Bgra8888:
                { ZoneScoped;
                    SkPixmap pixmap;
                    if(img->peekPixels(&pixmap)) {
                        if (!bmpEncoder.encode(static_cast<SkWStream *>(&stream), pixmap.addr32())) {
                            fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                        }
                    }
                }
                break;
            case kRawFrameOutputFormat_Png:
                { ZoneScoped;
                    SkPixmap pixmap;
                    if(img->peekPixels(&pixmap)) {
                        if (!SkPngEncoder::Encode(static_cast<SkWStream *>(&stream), pixmap, pngOptions)) {
                            fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                        }
                    }
                }
                break;
        }

        {
            ImGuiIO& io = ImGui::GetIO();
            double currentTime = SkTime::GetSecs();
            io.DeltaTime = static_cast<float>(currentTime - previousTime);
            previousTime = currentTime;
            frame++;

            const auto fps = io.Framerate;
            //fprintf(stderr,"Application average %.3f ms/frame (%.1f FPS) delta=%.3f ms\n", 1000.0f / fps, fps, io.DeltaTime*1.0e3);
        }
    }

    if(opts.fffiInterpreter) {
        render_cleanup();
    }
    ImGui::DestroyContext();

    return 0;
}

App::App() {
    fTotalVectorCmdSerializedSize = 0;
    fSkpBytesWritten = 0;
    fSvgBytesWritten = 0;
    fPngBytesWritten = 0;
    fBackgroundColor = SK_ColorRED;
    fFffiInterpreter = false;
    fUseVectorCmd = false;
}