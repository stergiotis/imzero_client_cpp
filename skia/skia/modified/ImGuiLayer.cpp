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

ImGuiLayer::ImGuiLayer(bool standalone) : fWindow(nullptr), fSvgBytesWritten(0), fSkpBytesWritten(0), fPngBytesWritten(0), fStandalone(standalone), fTotalVectorCmdSerializedSize(0) {
    // ImGui initialization:
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

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

    if(!fStandalone) {
        render_init();
    }

    // TODO remove this when switching to skia only backend
    build_ImFontAtlas(*io.Fonts, fFontPaint);

    fVectorCmdSkiaRenderer.setVertexDrawPaint(&fFontPaint);
    fVectorCmdSkiaRenderer.setParagraphHandler(ImGui::paragraph);

    fSkiaBackendActive = true;
}

ImGuiLayer::~ImGuiLayer() {
    if(!fStandalone) {
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

void ImGuiLayer::skiaWidget(const ImVec2& size, SkiaWidgetFunc func) {
    intptr_t funcIndex = fSkiaWidgetFuncs.size();
    fSkiaWidgetFuncs.push_back(func);
    ImGui::Image((ImTextureID)funcIndex, size);
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

void ImGuiLayer::drawImDrawData(SkCanvas &canvas) {
    // Then we fetch the most recent data, and convert it so we can render with Skia
    const ImDrawData* drawData = ImGui::GetDrawData();
    SkTDArray<SkPoint> pos;
    SkTDArray<SkPoint> uv;
    SkTDArray<SkColor> color;

    for (int i = 0; i < drawData->CmdListsCount; ++i) {
        const ImDrawList* drawList = drawData->CmdLists[i];

        // De-interleave all vertex data (sigh), convert to Skia types
        pos.clear(); uv.clear(); color.clear();
        for (int j = 0; j < drawList->VtxBuffer.size(); ++j) {
            const ImDrawVert& vert = drawList->VtxBuffer[j];
            pos.push_back(SkPoint::Make(vert.pos.x, vert.pos.y));
            uv.push_back(SkPoint::Make(vert.uv.x, vert.uv.y));
            color.push_back(vert.col);
        }
        // ImGui colors are RGBA
#ifndef IMGUI_USE_BGRA_PACKED_COLOR
        SkSwapRB(color.begin(), color.begin(), color.size());
#endif

        int indexOffset = 0;

        // Draw everything with canvas.drawVertices...
        for (int j = 0; j < drawList->CmdBuffer.size(); ++j) {
            const ImDrawCmd* drawCmd = &drawList->CmdBuffer[j];

            SkAutoCanvasRestore acr(&canvas, true);

            // TODO: Find min/max index for each draw, so we know how many vertices (sigh)
            if (drawCmd->UserCallback) {
                drawCmd->UserCallback(drawList, drawCmd);
            } else {
                auto idIndex = reinterpret_cast<intptr_t>(drawCmd->TextureId);
                if (idIndex < fSkiaWidgetFuncs.size()) {
                    // Small image IDs are actually indices into a list of callbacks. We directly
                    // examing the vertex data to deduce the image rectangle, then reconfigure the
                    // canvas to be clipped and translated so that the callback code gets to use
                    // Skia to render a widget in the middle of an ImGui panel.
                    ImDrawIdx rectIndex = drawList->IdxBuffer[indexOffset];
                    SkPoint tl = pos[rectIndex], br = pos[rectIndex + 2];
                    canvas.clipRect(SkRect::MakeLTRB(tl.fX, tl.fY, br.fX, br.fY));
                    canvas.translate(tl.fX, tl.fY);
                    fSkiaWidgetFuncs[static_cast<int>(idIndex)](&canvas);
                } else {
                    auto paint = static_cast<SkPaint*>(drawCmd->TextureId);
                    SkASSERT(paint);

                    canvas.clipRect(SkRect::MakeLTRB(drawCmd->ClipRect.x, drawCmd->ClipRect.y,
                                                      drawCmd->ClipRect.z, drawCmd->ClipRect.w));
                    auto vtxOffset = drawCmd->VtxOffset;
                    auto vertices = SkVertices::MakeCopy(SkVertices::kTriangles_VertexMode,
                                                         static_cast<int>(drawList->VtxBuffer.size() - vtxOffset),
                                                         pos.begin() + vtxOffset,
                                                         uv.begin() + vtxOffset,
                                                         color.begin() + vtxOffset,
                                                         static_cast<int>(drawCmd->ElemCount),
                                                         drawList->IdxBuffer.begin() + indexOffset);
                    canvas.drawVertices(vertices, SkBlendMode::kModulate, *paint);
                }
                indexOffset += static_cast<int>(drawCmd->ElemCount);
            }
        }
    }

    fSkiaWidgetFuncs.clear();
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

void ImGuiLayer::onPaint(SkSurface* surface) { ZoneScoped

    ImGui::skiaActive = fSkiaBackendActive;
    auto renderMode = fVectorCmdSkiaRenderer.getRenderMode();
    resetReceiveStat();
    resetSendStat();
    if(fStandalone) { ZoneScoped("demo window");
        ImGui::ShowDemoWindow();
    } else { ZoneScopedN("render fffi commands")
        render_render();
    }

    SaveFormatE saveFormat = SaveFormatE_None;
    if(ImGui::Begin("ImZeroSkia Settings")) { ZoneScoped
        fImZeroSkiaSetupUi.render(saveFormat, fVectorCmdSkiaRenderer, fSkiaBackendActive,
                                  fTotalVectorCmdSerializedSize, totalSentBytes+totalReceivedBytes,
                                  fSkpBytesWritten,fSvgBytesWritten, fPngBytesWritten,
                                  fWindow->width(), fWindow->height()
                                  );
    }
    ImGui::End();
    ImGui::Render();

    if(saveFormat != SaveFormatE_None) {
        switch(saveFormat) {
            case SaveFormatE_SKP: { ZoneScoped
                constexpr auto path = "/tmp/skiaBackend.skp";

                SkPictureRecorder skiaRecorder;
                auto skiaCanvas = skiaRecorder.beginRecording(SkIntToScalar(fWindow->width()),
                                                              SkIntToScalar(fWindow->height()));

                skiaCanvas->clear(SK_ColorDKGRAY);
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
            case SaveFormatE_SVGNoFont: { ZoneScoped
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
            case SaveFormatE_PNG: { ZoneScoped
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
            case SaveFormatE_VECTORCMD: { ZoneScoped
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
    } else if(fSkiaBackendActive) { ZoneScoped
        auto skiaCanvas = surface->getCanvas();

        skiaCanvas->clear(SK_ColorTRANSPARENT);
    
        skiaCanvas->save();
        drawImGuiVectorCmdsFB(*skiaCanvas);
        //renderImDrawData(*skiaCanvas);
        skiaCanvas->restore();
    } else { ZoneScoped
        drawImDrawData(*surface->getCanvas());
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