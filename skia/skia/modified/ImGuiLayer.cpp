/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#include "ImGuiLayer.h"

#include "include/svg/SkSVGCanvas.h"
#include "include/encode/SkPngEncoder.h"
#include "tracy/Tracy.hpp"

//#include "tools/trace/SkPerfettoTrace.h"
#include "tools/trace/ChromeTracingTracer.h"
#include "tools/trace/SkDebugfTracer.h"
#include "src/core/SkATrace.h"

#include <chrono>
#include <cstdint>

#include "../src/render.h"

using namespace sk_app;

#include "imgui_internal.h"
#include "../skiaTracyTracer.h"
#include "marshalling/receive.h"
#include "marshalling/send.h"
#include "buildinfo.gen.h"

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

template <typename T>
static inline void applyFlag(int &flag,T val,bool v) {
    if(v) {
        flag |= val;
    } else {
        flag &= ~(val);
    }
}

ImGuiLayer::ImGuiLayer(const CliOptions *opts) : fWindow(nullptr), fSvgBytesWritten(0), fSkpBytesWritten(0), fPngBytesWritten(0), ffffiInterpreter(opts->fffiInterpreter), fTotalVectorCmdSerializedSize(0), fBackground(SK_ColorTRANSPARENT), fUseVectorCmd(true) {
    // ImGui initialization:
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    applyFlag(io.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard, opts->imguiNavKeyboard);
    applyFlag(io.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad, opts->imguiNavGamepad);
    applyFlag(io.ConfigFlags, ImGuiConfigFlags_DockingEnable, opts->imguiDocking);

    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    if(opts->backgroundColorRGBA != nullptr && strlen(opts->backgroundColorRGBA) == 8) {
        auto const n = static_cast<uint32_t>(strtoul(opts->backgroundColorRGBA,nullptr,16));
        uint8_t a = n & 0xff;
        uint8_t b = (n >> 8) & 0xff;
        uint8_t g = (n >> 16) & 0xff;
        uint8_t r = (n >> 24) & 0xff;
        fBackground = SkColorSetARGB(a,r,g,b);
    }

    fUseVectorCmd = opts->vectorCmd;

    // Keymap...
    io.KeyMap[ImGuiKey_Tab]        = (int)skui::Key::kTab;
    io.KeyMap[ImGuiKey_LeftArrow]  = (int)skui::Key::kLeft;
    io.KeyMap[ImGuiKey_RightArrow] = (int)skui::Key::kRight;
    io.KeyMap[ImGuiKey_UpArrow]    = (int)skui::Key::kUp;
    io.KeyMap[ImGuiKey_DownArrow]  = (int)skui::Key::kDown;
    io.KeyMap[ImGuiKey_PageUp]     = (int)skui::Key::kPageUp;
    io.KeyMap[ImGuiKey_PageDown]   = (int)skui::Key::kPageDown;
    io.KeyMap[ImGuiKey_Home]       = (int)skui::Key::kHome;
    io.KeyMap[ImGuiKey_End]        = (int)skui::Key::kEnd;
    io.KeyMap[ImGuiKey_Delete]     = (int)skui::Key::kDelete;
    io.KeyMap[ImGuiKey_Backspace]  = (int)skui::Key::kBack;
    io.KeyMap[ImGuiKey_Enter]      = (int)skui::Key::kOK;
    io.KeyMap[ImGuiKey_Escape]     = (int)skui::Key::kEscape;
    io.KeyMap[ImGuiKey_A]          = (int)skui::Key::kA;
    io.KeyMap[ImGuiKey_C]          = (int)skui::Key::kC;
    io.KeyMap[ImGuiKey_V]          = (int)skui::Key::kV;
    io.KeyMap[ImGuiKey_X]          = (int)skui::Key::kX;
    io.KeyMap[ImGuiKey_Y]          = (int)skui::Key::kY;
    io.KeyMap[ImGuiKey_Z]          = (int)skui::Key::kZ;

    if(ffffiInterpreter) {
        render_init();
    }

    // TODO remove this when switching to skia only backend
    build_ImFontAtlas(*io.Fonts, fFontPaint);

    fVectorCmdSkiaRenderer.setVertexDrawPaint(&fFontPaint);
    fVectorCmdSkiaRenderer.setParagraphHandler(ImGui::paragraph);
    RenderModeE mode = 0;
    if(opts->backdropFilter) {
        mode |= RenderModeE_BackdropBlur;
    }
    if(opts->sketchFilter) {
        mode |= RenderModeE_Sketch;
    }
    fVectorCmdSkiaRenderer.changeRenderMode(mode);
}

ImGuiLayer::~ImGuiLayer() {
    if(ffffiInterpreter) {
        render_cleanup();
    }
    ImGui::DestroyContext();
    //if(eventTracer != nullptr) {
    //    delete eventTracer;
    //}
}

void ImGuiLayer::setScaleFactor(float scaleFactor) {
    ImGui::GetStyle().ScaleAllSizes(scaleFactor);

    auto f = ImGui::GetIO().FontDefault;
    if(f != nullptr && f->IsLoaded()) {
        ImFontAtlas& atlas = *ImGui::GetIO().Fonts;
        atlas.Clear();
        ImFontConfig cfg;
        cfg.SizePixels = 13 * scaleFactor;
        atlas.AddFontDefault(&cfg);
        build_ImFontAtlas(atlas, fFontPaint);
        // FIXME
    }
}

#if defined(SK_BUILD_FOR_UNIX)
static const char* get_clipboard_text(void* user_data) {
    auto w = static_cast<Window*>(user_data);
    return w->getClipboardText();
}

static void set_clipboard_text(void* user_data, const char* text) {
    auto w = static_cast<Window*>(user_data);
    w->setClipboardText(text);
}
#endif

void ImGuiLayer::onAttach(Window* window) {
    fWindow = window;

#if defined(SK_BUILD_FOR_UNIX)
    ImGuiIO& io = ImGui::GetIO();
    io.ClipboardUserData = fWindow;
    io.GetClipboardTextFn = get_clipboard_text;
    io.SetClipboardTextFn = set_clipboard_text;
#endif
}

bool ImGuiLayer::onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos.x = static_cast<float>(x);
    io.MousePos.y = static_cast<float>(y);
    if (skui::InputState::kDown == state) {
        io.MouseDown[0] = true;
    } else if (skui::InputState::kUp == state) {
        io.MouseDown[0] = false;
    }
    return io.WantCaptureMouse;
}

bool ImGuiLayer::onMouseWheel(float delta, int, int, skui::ModifierKey modifiers) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += delta;
    return io.WantCaptureMouse;
}

void ImGuiLayer::onPrePaint() {
    // Update ImGui input
    ImGuiIO& io = ImGui::GetIO();

#if 1
    static double previousTime = 0.0;
    double currentTime = SkTime::GetSecs();
    io.DeltaTime = static_cast<float>(currentTime - previousTime);
#else
    static auto previousTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = currentTime - previousTime;
    io.DeltaTime = delta.count();
#endif
    previousTime = currentTime;

    io.DisplaySize.x = static_cast<float>(fWindow->width());
    io.DisplaySize.y = static_cast<float>(fWindow->height());

    io.KeyAlt   = io.KeysDown[static_cast<int>(skui::Key::kOption)];
    io.KeyCtrl  = io.KeysDown[static_cast<int>(skui::Key::kCtrl)];
    io.KeyShift = io.KeysDown[static_cast<int>(skui::Key::kShift)];
    io.KeySuper = io.KeysDown[static_cast<int>(skui::Key::kSuper)];

    ImGui::NewFrame();
}

void ImGuiLayer::drawImGuiVectorCmdsFB(SkCanvas &canvas) {
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

void ImGuiLayer::onPaint(SkSurface* surface) { ZoneScoped;
    ImGui::useVectorCmd = fUseVectorCmd;
    auto renderMode = fVectorCmdSkiaRenderer.getRenderMode();
    resetReceiveStat();
    resetSendStat();
    if(ffffiInterpreter) { ZoneScopedN("render fffi commands");
        render_render();
    } else { ZoneScopedN("demo window");
        ImGui::ShowDemoWindow();
    }

    SaveFormatE saveFormat = SaveFormatE_None;
    if(ImGui::Begin("ImZeroSkia Settings")) { ZoneScoped;
        fImZeroSkiaSetupUi.render(saveFormat, fVectorCmdSkiaRenderer, fUseVectorCmd,
                                  fTotalVectorCmdSerializedSize, totalSentBytes+totalReceivedBytes,
                                  fSkpBytesWritten, fSvgBytesWritten, fPngBytesWritten,
                                  fWindow->width(), fWindow->height()
                                  );
    }
    ImGui::End();
    ImGui::Render();

    if(saveFormat != SaveFormatE_None) {
        switch(saveFormat) {
            case SaveFormatE_SKP: { ZoneScoped;
                constexpr auto path = "/tmp/skiaBackend.skp";

                SkPictureRecorder skiaRecorder;
                auto skiaCanvas = skiaRecorder.beginRecording(SkIntToScalar(fWindow->width()),
                                                              SkIntToScalar(fWindow->height()));

                skiaCanvas->clear(fBackground);
                skiaCanvas->save();
                drawImGuiVectorCmdsFB(*skiaCanvas);
                skiaCanvas->restore();

                sk_sp<SkPicture> picture = skiaRecorder.finishRecordingAsPicture();
                SkFILEWStream skpStream(path);
                picture->serialize(&skpStream);
                fSkpBytesWritten = skpStream.bytesWritten();
                fprintf(stderr, "skp=%d\n", (int) fSkpBytesWritten);
                break;
            }
            case SaveFormatE_SVG: // fallthrough
            case SaveFormatE_SVGNoFont: { ZoneScoped;
                SkRect bounds = SkRect::MakeIWH(fWindow->width(), fWindow->height());
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
                const auto s = SkISize::Make(fWindow->width(), fWindow->height());
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
        skiaCanvas->clear(fBackground);
    
        skiaCanvas->save();
        drawImGuiVectorCmdsFB(*skiaCanvas);
        skiaCanvas->restore();
    }

    FrameMark;
}

bool ImGuiLayer::onKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers) {
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[static_cast<int>(key)] = (skui::InputState::kDown == state);
    return io.WantCaptureKeyboard;
}

bool ImGuiLayer::onChar(SkUnichar c, skui::ModifierKey modifiers) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput) {
        if (c > 0 && c < 0x10000) {
            io.AddInputCharacter(c);
        }
        return true;
    }
    return false;
}