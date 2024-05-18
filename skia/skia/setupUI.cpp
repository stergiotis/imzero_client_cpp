#include "setupUI.h"
#include "imgui_internal.h"
#include "tracy/Tracy.hpp"

#include "src/core/SkATrace.h"
#include "tools/trace/SkDebugfTracer.h"
#include "tools/trace/ChromeTracingTracer.h"
#include "skiaTracyTracer.h"
#include "implot.h"

ImZeroSkiaSetupUI::ImZeroSkiaSetupUI() {
    fontMetricsText[0] = 'H';
    fontMetricsText[1] = 'f';
    fontMetricsText[2] = 'm';
    fontMetricsText[3] = 'x';
    fontMetricsText[4] = 'c';
    fontMetricsText[5] = 'j';
    fontMetricsText[6] = '\0';
    fontMetricsSize = 200.0f;
    colSize = ImVec4(1.0f,1.0f,1.0f,1.0f);
    colAscent = ImVec4(1.0f,1.0f,1.0f,1.0f);
    colDescent = ImVec4(1.0f,1.0f,1.0f,1.0f);
    colLeading = ImVec4(1.0f,1.0f,1.0f,1.0f);
    colXHeight = ImVec4(1.0f,1.0f,1.0f,1.0f);
    colCapHeight = ImVec4(1.0f,1.0f,1.0f,1.0f);
}
ImZeroSkiaSetupUI::~ImZeroSkiaSetupUI() = default;

static void ResetImGuiFramerateMovingAverage() {
    // Credits: Copied from https://github.com/ocornut/imgui/issues/5236
    ImGuiContext& g = *ImGui::GetCurrentContext();
    memset(g.FramerateSecPerFrame, 0, sizeof(g.FramerateSecPerFrame));
    g.FramerateSecPerFrameIdx = g.FramerateSecPerFrameCount = 0;
    g.FramerateSecPerFrameAccum = 0.0f;
}
static void helpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
void ImZeroSkiaSetupUI::render(SaveFormatE &saveFormat, VectorCmdSkiaRenderer &vectorCmdSkiaRenderer, bool &skiaBackendActive,
                               size_t totalVectorCmdSerializedSz, size_t totalFffiSz,
                               size_t skpBytes, size_t svgBytes, size_t pngBytes, int windowW, int windowH
                               ) { ZoneScoped;
    if(ImGui::CollapsingHeader("Skia Backend")) {
        auto renderMode = vectorCmdSkiaRenderer.getRenderMode();
        ImGui::Checkbox("Render##Skia",&skiaBackendActive);

#ifdef RENDER_MODE_SKETCH_ENABLED
        if(ImGui::CheckboxFlags("Sketch",&renderMode,RenderModeE_Sketch)) {
            vectorCmdSkiaRenderer.changeRenderMode(renderMode);
        }
        if(ImGui::CheckboxFlags("SVG",&renderMode,RenderModeE_SVG)) {
            vectorCmdSkiaRenderer.changeRenderMode(renderMode);
        }
        ImGui::SameLine();
        helpMarker("RENDER_MODE_SKETCH_ENABLED compile time option is set");
#else
        ImGui::TextUnformatted("RENDER_MODE_SKETCH_ENABLED compile time option is not set");
#endif
#ifdef RENDER_MODE_BACKDROP_FILTER_ENABLED
        if(ImGui::CheckboxFlags("Backdrop Filter",&renderMode,RenderModeE_BackdropBlur)) {
            vectorCmdSkiaRenderer.changeRenderMode(renderMode);
        }
        ImGui::SameLine();
        helpMarker("RENDER_MODE_BACKDROP_FILTER_ENABLED compile time option is set");
#else
        ImGui::TextUnformatted("RENDER_MODE_BACKDROP_FILTER_ENABLED compile time option is not set");
#endif
    }
    saveFormat = SaveFormatE_None;
    if(ImGui::CollapsingHeader("(Vector) Screenshots")) {
        ImGui::Text("serialized flatbuffer verctor cmd size: %d Bytes", static_cast<int>(totalVectorCmdSerializedSz));
        ImGui::Text("fffi cmd size: %d Bytes",static_cast<int>(totalFffiSz));
        ImGui::Separator();

        if(ImGui::Button("Save Snapshot to /tmp/skiaBackend.skp")) {
            saveFormat = SaveFormatE_SKP;
        }
        if(ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s","Open SKP files with `viewer --skps PATH_TO_SKP --slide SKP_FILE");
        }
        if(skpBytes > 0) {
            ImGui::Text("skp file size: %d Bytes", static_cast<int>(skpBytes));
        }
        if(ImGui::Button("Save Snapshot to /tmp/skiaBackend.svg")) {
            saveFormat = SaveFormatE_SVG;
        }
        if(ImGui::Button("Save Snapshot to /tmp/skiaBackend.nofont.svg")) {
            saveFormat = SaveFormatE_SVGNoFont;
        }
        if(svgBytes > 0) {
            ImGui::Text("svg file size: %d Bytes", static_cast<int>(svgBytes));
        }
        if(ImGui::Button("Save Snapshot to /tmp/skiaBackend.png")) {
            saveFormat = SaveFormatE_PNG;
        }
        if(pngBytes > 0) {
            ImGui::Text("png file size: %d Bytes", static_cast<int>(pngBytes));
        }
        if(ImGui::Button("Save Snapshot to /tmp/skiaBackend.flatbuffers")) {
            saveFormat = SaveFormatE_VECTORCMD;
        }
    }

    if(ImGui::CollapsingHeader("Metrics")) {
        ImGuiIO& io = ImGui::GetIO();
        const auto fps = io.Framerate;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);
        ImGui::SameLine();
        if(ImGui::SmallButton("Reset")) {
            ResetImGuiFramerateMovingAverage();
        }
        ImGui::Text("Current DeltaTime %.3f ms", io.DeltaTime*1000.0f);
        ImGui::Text("%d visible windows", io.MetricsRenderWindows);
        const auto resF =  static_cast<float>(windowW*windowH);
        const auto resFRel1080p =  resF/(1920.0f*1080.0f);
        const auto resFRel720p =  resF/(1280.0f*720.0f);
        ImGui::Text("Resolution %dx%d Pixels = %.2f x 1080p = %.2f x 720p", windowW,windowH,resFRel1080p,resFRel720p);
    }

    if(ImGui::CollapsingHeader("Build Information")) {
#ifdef TRACY_ENABLE
        ImGui::TextUnformatted("TRACY_ENABLE is set");
        ImGui::SameLine();
        helpMarker("This application can be profiled using tracy (see github.com/wolfpld/tracy).");
#else
        ImGui::TextUnformatted("TRACY_ENABLE is not set");
        ImGui::SameLine();
        helpMarker("This application can not be profiled using trace (see github.com/wolfpld/tracy).");
#endif
#ifdef IMZERO_DEBUG_BUILD
        ImGui::TextUnformatted("IMZERO_DEBUG_BUILD is set");
        ImGui::SameLine();
        helpMarker("This application may performed sub-par to the release build.");
#else
        ImGui::TextUnformatted("IMZERO_DEBUG_BUILD is not set");
        ImGui::SameLine();
        helpMarker("This application is a release build and may not be suitable for debugging your application");
#endif
#ifdef NDEBUG
        ImGui::TextUnformatted("NDEBUG is set");
        ImGui::SameLine();
        helpMarker("This application will not evaluate assertions (i.e. assert(...))");
#else
        ImGui::TextUnformatted("NDEBUG is not set");
        ImGui::SameLine();
        helpMarker("This application is evaluating assertions (i.e. assert(...))");
#endif
    }

    if(ImGui::CollapsingHeader("Font Metrics")) { ZoneScoped;
        ImGui::InputText("Text",fontMetricsText,sizeof(fontMetricsText));

        ImGui::DragFloat("Size",&fontMetricsSize,1.0f,3.0f,400.0f);
        ImGui::DragFloat("Global Font Scale",&ImGui::GetIO().FontGlobalScale,0.01f,0.01f,10.0f);

        auto len = strlen(fontMetricsText);
        if(len > 0 && fontMetricsSize > 1.0f) {
            SkRect bounds;
            auto f = ImGui::skiaFont.makeWithSize(SkScalar(fontMetricsSize));
            SkScalar advanceWidth = f.measureText(fontMetricsText,len,SkTextEncoding::kUTF8, &bounds);
            SkFontMetrics metrics{};
            f.getMetrics(&metrics);
            bounds.sort();
            auto actualSize = fabs(metrics.fAscent) + fabs(metrics.fDescent);
            //auto dy = fontMetricsSize-SkScalarToFloat(metrics.fDescent);
            auto dy = fabs(SkScalarToFloat(metrics.fAscent)) + fontMetricsSize*ImGui::skiaFontDyFudge;

            ImGui::DragFloat("dy fudge",&ImGui::skiaFontDyFudge,0.001f,-1.0f,1.0f);
            dy += fontMetricsSize*ImGui::skiaFontDyFudge;

            {
                auto a = fabs(metrics.fAscent)-fabs(metrics.fCapHeight);
                if(a == fabs(metrics.fDescent)) {
                    ImGui::TextUnformatted("|ascent|-|capheight| == |descent|   => vertical centering will be easy!");
                } else {
                    ImGui::Text("%f = |ascent|-|capheight| != |descent| = %f   => vertical centering will be hard!", a, metrics.fDescent);
                }

                if(actualSize == fabs(fontMetricsSize)) {
                    ImGui::TextUnformatted("|ascent|+|descent| == |size|   => easy");
                } else {
                    ImGui::Text("%f = |ascent|+|descent| != |size| = %f   => hard", fabs(metrics.fAscent)+fabs(metrics.fDescent), fontMetricsSize);
                }
            }

            if(ImGui::BeginTable("metrics",5,ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Metric");
                ImGui::TableSetupColumn("Value");
                ImGui::TableSetupColumn("Description");
                ImGui::TableSetupColumn("Sign");
                ImGui::TableSetupColumn("Color");
                ImGui::TableSetupScrollFreeze(0,1);
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("size");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",fontMetricsSize);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("font size");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("positive");
                ImGui::TableNextColumn();
                ImGui::ColorEdit4("size",&colSize.x);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("ascent");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",metrics.fAscent);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("distance to reserve above baseline");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("typically negative");
                ImGui::TableNextColumn();
                ImGui::ColorEdit4("ascent",&colAscent.x);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("descent");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",metrics.fDescent);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("distance to reserve below baseline");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("typically positive");
                ImGui::TableNextColumn();
                ImGui::ColorEdit4("descent",&colDescent.x);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("leading");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",metrics.fLeading);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("distance to add between lines");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("typically positive or zero");
                ImGui::TableNextColumn();
                ImGui::ColorEdit4("leading",&colLeading.x);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("x height");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",metrics.fXHeight);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("height of lower-case 'x'");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("zero if unknown, typically negative");
                ImGui::TableNextColumn();
                ImGui::ColorEdit4("x height",&colXHeight.x);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("cap height");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",metrics.fCapHeight);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("height of an upper-case letter");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("zero if unknown, typically negative");
                ImGui::TableNextColumn();
                ImGui::ColorEdit4("cap height",&colCapHeight.x);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("bounds top");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",bounds.top());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("bounds left");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",bounds.left());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("bounds bottom");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",bounds.bottom());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("bounds right");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",bounds.right());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("baseline y");
                ImGui::TableNextColumn();
                ImGui::Text("%.3f",dy);

                ImGui::EndTable();
            }

            ImVec2 size(bounds.width()*1.2f+7.0f*5.0f, bounds.height()*1.2f);
            ImGui::InvisibleButton("canvas", size);
            auto p0 = ImGui::GetItemRectMin();
            auto p1 = ImGui::GetItemRectMax();
            auto dl = ImGui::GetWindowDrawList();
            dl->PushClipRect(p0, p1);
            ImVec2 tl = p0+ImVec2(7.0f*5.0f,0.0f);
            auto bl0 = p0+ImVec2(0.0f,dy);
            auto bl = tl+ImVec2(0.0f,dy);
            const auto colBlue = IM_COL32(0x11,0x99,0xff,0xff);
            const auto colRed = IM_COL32(0x99,0x11,0x00,0xff);
            const auto colGreen = IM_COL32(0x00,0x99,0x11,0xff);
            dl->AddRect(p0,p1,colRed,0.0f,0,1.0f);
            dl->AddRect(tl+ImVec2(bounds.left(),bounds.top()),tl+ImVec2(bounds.right(),bounds.bottom()),colBlue,0.0f,0,1.0f);
            dl->AddRect(tl+ImVec2(bounds.left(),bounds.top()+dy),tl+ImVec2(bounds.right(),bounds.bottom()+dy),colBlue,0.0f,0,1.0f);
            dl->AddLine(bl,bl+ImVec2(advanceWidth,0.0f),colGreen,1.0f);
            dl->AddText(nullptr,fontMetricsSize,tl,colBlue,fontMetricsText,fontMetricsText+len);

            auto dx = 5.0f;
            dl->AddLine(p0+ImVec2(dx,0.0f),p0+ImVec2(dx,fontMetricsSize),ImColor(colSize),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,metrics.fAscent),ImColor(colAscent),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,metrics.fDescent),ImColor(colDescent),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,metrics.fLeading),ImColor(colLeading),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,-fabs(metrics.fXHeight)),ImColor(colXHeight),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,-fabs(metrics.fCapHeight)),ImColor(colCapHeight),2.0f);
            dl->PopClipRect();
        }
    }


    if(ImGui::CollapsingHeader("Skia Tracing")) {
        if(fEventTracer != nullptr) {
            ImGui::TextUnformatted("tracing is in progress...");
            /*if(ImGui::Button("stop tracing")) {
                delete eventTracer;
                eventTracer = nullptr;
            }*/
        } else {
            int opt = 0;
            if(ImGui::Button("atrace (android)")) {
                opt = 1;
            }
            if(ImGui::Button("debugf")) {
                opt = 2;
            }
            /*if(ImGui::Button("perfetto")) {
                opt = 3;
            }*/
            if(ImGui::Button("chromium (/tmp/skiatrace.json)")) {
                opt = 4;
            }
#ifndef TRACY_ENABLE
            ImGui::BeginDisabled();
#endif
            if(ImGui::Button("tracy")) {
                opt = 5;
            }
#ifndef TRACY_ENABLE
            ImGui::EndDisabled();
#endif

            switch(opt) {
                case 1:
                    fEventTracer = new SkATrace();
                    break;
                case 2:
                    fEventTracer = new SkDebugfTracer();
                    break;
                case 3:
                    //eventTracer = new SkPerfettoTrace();
                    break;
                case 4:
                    fEventTracer = new ChromeTracingTracer("/tmp/skiatrace.json");
                    break;
                case 5:
                    fEventTracer = new skiaTracyTracer();
                    break;
            }
            if(fEventTracer != nullptr) {
                // skia will take ownership of eventTracer
                SkAssertResult(SkEventTracer::SetInstance(fEventTracer, false));
            }
        }
    }
    if(ImGui::CollapsingHeader("Paragraph")) {
        ImGui::TextUnformatted("this is a multiline\ntext with many words that will\nhopefully form a paragraph.");
    }

    if(ImGui::CollapsingHeader("Paragraph Cache")) {
        int count = 0;
        ImGui::paragraph->getCacheStatistics(count);
        ImGui::Text("cache count: %d",count);
        if(ImGui::Button("reset cache")) {
            ImGui::paragraph->resetCache();
        }
        if(ImGui::Button("on")) {
            ImGui::paragraph->setCacheEnable(true);
        }
        if(ImGui::Button("off")) {
            ImGui::paragraph->setCacheEnable(false);
        }
    }
}
