#include "imgui_skia_setup_ui.h"

#include "include/core/SkSurface.h"
#include "include/core/SkStream.h"
#include "include/core/SkPicture.h"

#include "include/core/SkColorSpace.h"
#if defined(linux) || defined(__linux) || defined(__linux__)
#include <sys/time.h>
#endif
#include "imgui_internal.h"
#include "tracy/Tracy.hpp"

const char *ImGuiSkia::GetFrameExportFormatName(const FrameExportFormatE format) {
    size_t dummy;
    return ImGuiSkia::GetFrameExportFormatName(format, dummy);
}
const char *ImGuiSkia::GetFrameExportFormatName(const FrameExportFormatE format, size_t &len) {
    switch (format) {
    case FrameExportFormatE_NoExport: len=strlen("none"); return "none";
    case FrameExportFormatE_SKP: len=strlen("skp"); return "skp";
    case FrameExportFormatE_SVG: len=strlen("svg"); return "svg";
    case FrameExportFormatE_SVG_TextAsPath: len=strlen("svg-text-as-path"); return "svg-text-as-path";
    case FrameExportFormatE_PNG: len=strlen("png"); return "png";
    case FrameExportFormatE_JPEG: len=strlen("jpeg"); return "jpeg";
    case FrameExportFormatE_VECTORCMD: len=strlen("vectorcmd"); return "vectorcmd";
    case FrameExportFormatE_Disabled: len=strlen("disabled"); return "disabled";
    default:
        len=strlen("<unknown>"); return "<unknown>";
    }
}
const char *ImGuiSkia::GetFrameExportFormatExtension(const FrameExportFormatE format) {
    size_t dummy;
    return ImGuiSkia::GetFrameExportFormatExtension(format, dummy);
}
const char *ImGuiSkia::GetFrameExportFormatExtension(const FrameExportFormatE format, size_t &len) {
    switch (format) {
    case FrameExportFormatE_NoExport: len=strlen("<none>"); return "<none>";
    case FrameExportFormatE_SKP: len=strlen(".skp"); return ".skp";
    case FrameExportFormatE_SVG:  len=strlen(".svg"); return ".svg";
    case FrameExportFormatE_SVG_TextAsPath:  len=strlen(".textAsPath.svg"); return ".textAsPath.svg";
    case FrameExportFormatE_PNG:  len=strlen(".png"); return ".png";
    case FrameExportFormatE_JPEG:  len=strlen(".jpeg"); return ".jpeg";
    case FrameExportFormatE_VECTORCMD:  len=strlen(".vectorcmd.fb"); return ".vectorcmd.fb";
    case FrameExportFormatE_Disabled:  len=strlen("<disabled>"); return "<disabled>";
    default:
        len=strlen("<unknown>"); return "<unknown>";
    }
}

ImGuiSkia::SetupUI::SetupUI() {
    fFontMetricsText[0] = 'H';
    fFontMetricsText[1] = 'f';
    fFontMetricsText[2] = 'm';
    fFontMetricsText[3] = 'x';
    fFontMetricsText[4] = 'c';
    fFontMetricsText[5] = 'j';
    fFontMetricsText[6] = '\0';
    fFontMetricsSize = 200.0f;
    fColSize = ImVec4(1.0f,1.0f,1.0f,1.0f);
    fColAscent = ImVec4(1.0f,1.0f,1.0f,1.0f);
    fColDescent = ImVec4(1.0f,1.0f,1.0f,1.0f);
    fColLeading = ImVec4(1.0f,1.0f,1.0f,1.0f);
    fColXHeight = ImVec4(1.0f,1.0f,1.0f,1.0f);
    fColCapHeight = ImVec4(1.0f,1.0f,1.0f,1.0f);

    {
        // chromium --enable-gpu-benchmarking --no-sandbox
        // javascript: chrome.gpuBenchmarking.printToSkPicture("/tmp/out.skp")
        auto const stream = SkStream::MakeFromFile("/tmp/out.skp/layer_0.skp");
        if(stream.get()) {
            fSamplePicture = SkPicture::MakeFromStream(stream.get());
        } else {
            fSamplePicture = nullptr;
        }
    }

    {
        if (fSamplePicture.get()) {
            constexpr auto s = SkISize::Make(1000, 1000);
            const auto c = SkColorInfo(kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
            fSampleSurface = SkSurfaces::Raster(SkImageInfo::Make(s, c));
            const auto canvas = fSampleSurface->getCanvas();
            canvas->drawPicture(fSamplePicture);
        } else {
            constexpr auto s = SkISize::Make(100, 100);
            const auto c = SkColorInfo(kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
            fSampleSurface = SkSurfaces::Raster(SkImageInfo::Make(s, c));
            //auto canvas = fSampleSurface->getCanvas();
        }
    }
}

ImGuiSkia::SetupUI::~SetupUI() = default;

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
void ImGuiSkia::SetupUI::render(FrameExportFormatE &exportFormat, VectorCmdSkiaRenderer &vectorCmdSkiaRenderer, bool &useVectorCmd,
                                size_t totalVectorCmdSerializedSize, size_t fTotalFffiSz,
                                size_t skpBytes, size_t fbBytes, size_t svgBytes, size_t pngBytes, size_t jpegBytes, int windowW, int windowH,
                                SkFontMgr *fontMgr,
                                const char *basePath) { ZoneScoped;
#if defined(linux) || defined(__linux) || defined(__linux__)
    {
        struct timeval tv;
        struct timezone tz;

        gettimeofday(&tv, &tz);

        char buf[sizeof "9999-12-31T23:59:59.999+0000000"];
        size_t bufsize = sizeof buf;
        size_t off = 0;
        tm *local = localtime(&tv.tv_sec);
        off = strftime(buf, bufsize, "%FT%T", local); // same as "%Y-%m-%dT%H:%M:%S"
        off += snprintf(buf+off, bufsize-off, ".%06ld", tv.tv_usec);
        off += strftime(buf+off, bufsize-off, "%z", local);

        ImGui::TextUnformatted(buf);
    }
#endif

    if (ImGui::CollapsingHeader("Image")) {
        //ImGui::Image(reinterpret_cast<ImTextureID>(fSamplePicture.get()),ImVec2(0.0f,0.0f));
        ImGui::Image(reinterpret_cast<ImTextureID>(fSampleSurface.get()),ImVec2(0.0f,0.0f));
    }

    if(ImGui::CollapsingHeader("Skia Backend")) {
        auto renderMode = vectorCmdSkiaRenderer.getRenderMode();
        ImGui::Checkbox("Use vector commands##Skia",&useVectorCmd);

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
    if(exportFormat != FrameExportFormatE_Disabled) {
        exportFormat = FrameExportFormatE_NoExport;
        if(ImGui::CollapsingHeader("(Vector) Screenshots")) {
            ImGui::Text("serialized flatbuffer vector cmd size: %d Bytes", static_cast<int>(totalVectorCmdSerializedSize));
            ImGui::Text("fffi cmd size: %d Bytes",static_cast<int>(fTotalFffiSz));
            ImGui::Text("base path: %s", basePath);
            ImGui::Separator();

            if(ImGui::Button("Save SKP Snapshot")) {
                exportFormat = FrameExportFormatE_SKP;
            }
            if(ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s","Open SKP files with `viewer --skps PATH_TO_SKP --slide SKP_FILE");
            }
            if(skpBytes > 0) {
                ImGui::Text("%s file size: %d Bytes", GetFrameExportFormatExtension(FrameExportFormatE_SKP), static_cast<int>(skpBytes));
            }
            if(ImGui::Button("Save SVG Snapshot")) {
                exportFormat = FrameExportFormatE_SVG;
            }
            if(ImGui::Button("Save SVG (Text as Path) Snapshot")) {
                exportFormat = FrameExportFormatE_SVG_TextAsPath;
            }
            if(svgBytes > 0) {
                ImGui::Text("%s/%s file size: %d Bytes", GetFrameExportFormatExtension(FrameExportFormatE_SVG), GetFrameExportFormatExtension(FrameExportFormatE_SVG_TextAsPath), static_cast<int>(svgBytes));
            }
            if(ImGui::Button("Save PNG Snapshot")) {
                exportFormat = FrameExportFormatE_PNG;
            }
            if(pngBytes > 0) {
                ImGui::Text("%s file size: %d Bytes", GetFrameExportFormatExtension(FrameExportFormatE_PNG), static_cast<int>(pngBytes));
            }
            if(ImGui::Button("Save JPEG Snapshot")) {
                exportFormat = FrameExportFormatE_JPEG;
            }
            if(jpegBytes > 0) {
                ImGui::Text("%s file size: %d Bytes", GetFrameExportFormatExtension(FrameExportFormatE_JPEG), static_cast<int>(jpegBytes));
            }
            if(ImGui::Button("Save FB Snapshot")) {
                exportFormat = FrameExportFormatE_VECTORCMD;
            }
            if(fbBytes > 0) {
                ImGui::Text("%s file size: %d Bytes", GetFrameExportFormatExtension(FrameExportFormatE_VECTORCMD), static_cast<int>(fbBytes));
            }
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
#ifdef IMGUI_SKIA_DEBUG_BUILD
        ImGui::TextUnformatted("IMGUI_SKIA_DEBUG_BUILD is set");
        ImGui::SameLine();
        helpMarker("This application may performed sub-par to the release build.");
#else
        ImGui::TextUnformatted("IMGUI_SKIA_DEBUG_BUILD is not set");
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
        ImGui::InputText("Text",fFontMetricsText,sizeof(fFontMetricsText));

        ImGui::DragFloat("Size",&fFontMetricsSize,1.0f,3.0f,400.0f);
        ImGui::DragFloat("Global Font Scale",&ImGui::GetIO().FontGlobalScale,0.01f,0.01f,10.0f);

        auto len = strlen(fFontMetricsText);
        if(len > 0 && fFontMetricsSize > 1.0f) {
            SkRect bounds;
            auto f = ImGui::skiaFont.makeWithSize(SkScalarToFloat(fFontMetricsSize));
            SkScalar advanceWidth = f.measureText(fFontMetricsText,len,SkTextEncoding::kUTF8, &bounds);
            SkFontMetrics metrics{};
            f.getMetrics(&metrics);
            bounds.sort();
            auto actualSize = fabs(metrics.fAscent) + fabs(metrics.fDescent);
            //auto dy = fontMetricsSize-SkScalarToFloat(metrics.fDescent);
            auto dy = fabs(SkScalarToFloat(metrics.fAscent)) + fFontMetricsSize*ImGui::skiaFontDyFudge;

            ImGui::DragFloat("dy fudge",&ImGui::skiaFontDyFudge,0.001f,-1.0f,1.0f);
            dy += fFontMetricsSize*ImGui::skiaFontDyFudge;

            ImGui::DragFloat("font scale override",&ImGui::skiaFontScaleOverride,0.001f,-1.0f,1.0f);
            {
                auto a = fabs(metrics.fAscent)-fabs(metrics.fCapHeight);
                if(a == fabs(metrics.fDescent)) {
                    ImGui::TextUnformatted("|ascent|-|capheight| == |descent|   => vertical centering will be easy!");
                } else {
                    ImGui::Text("%f = |ascent|-|capheight| != |descent| = %f   => vertical centering will be hard!", a, metrics.fDescent);
                }

                if(actualSize == fabs(fFontMetricsSize)) {
                    ImGui::TextUnformatted("|ascent|+|descent| == |size|   => easy");
                } else {
                    ImGui::Text("%f = |ascent|+|descent| != |size| = %f   => hard", fabs(metrics.fAscent)+fabs(metrics.fDescent), fFontMetricsSize);
		    if(ImGui::Button("auto scale")) {
			    ImGui::skiaFontScaleOverride = fFontMetricsSize/(fabs(metrics.fAscent)+fabs(metrics.fDescent));
		    }
                }
            }
            if(f.isLinearMetrics()) {
                ImGui::TextUnformatted("linear scalable metrics");
            } else {
                ImGui::TextUnformatted("non-linear scaling metrics");
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
                ImGui::Text("%.3f",fFontMetricsSize);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("font size");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("positive");
                ImGui::TableNextColumn();
                ImGui::ColorEdit4("size",&fColSize.x);

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
                ImGui::ColorEdit4("ascent",&fColAscent.x);

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
                ImGui::ColorEdit4("descent",&fColDescent.x);

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
                ImGui::ColorEdit4("leading",&fColLeading.x);

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
                ImGui::ColorEdit4("x height",&fColXHeight.x);

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
                ImGui::ColorEdit4("cap height",&fColCapHeight.x);

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

            dl->PushClipRect(p0, p1,true);
            ImVec2 tl = p0+ImVec2(7.0f*5.0f,0.0f);
            auto bl0 = p0+ImVec2(0.0f,dy);
            auto bl = tl+ImVec2(0.0f,dy);
            constexpr auto colBlue = IM_COL32(0x11,0x99,0xff,0xff);
            constexpr auto colRed = IM_COL32(0x99,0x11,0x00,0xff);
            constexpr auto colGreen = IM_COL32(0x00,0x99,0x11,0xff);
            dl->AddRect(p0,p1,colRed,0.0f,0,1.0f);
            dl->AddRect(tl+ImVec2(bounds.left(),bounds.top()),tl+ImVec2(bounds.right(),bounds.bottom()),colBlue,0.0f,0,1.0f);
            dl->AddRect(tl+ImVec2(bounds.left(),bounds.top()+dy),tl+ImVec2(bounds.right(),bounds.bottom()+dy),colBlue,0.0f,0,1.0f);
            dl->AddLine(bl,bl+ImVec2(advanceWidth,0.0f),colGreen,1.0f);
            dl->AddText(nullptr,fFontMetricsSize,tl,colBlue,fFontMetricsText,fFontMetricsText+len);

            auto dx = 5.0f;
            dl->AddLine(p0+ImVec2(dx,0.0f),p0+ImVec2(dx,fFontMetricsSize),ImColor(fColSize),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,metrics.fAscent),ImColor(fColAscent),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,metrics.fDescent),ImColor(fColDescent),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,metrics.fLeading),ImColor(fColLeading),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,-fabs(metrics.fXHeight)),ImColor(fColXHeight),2.0f);
            dx += 5.0f;
            dl->AddLine(bl0+ImVec2(dx,0.0f),bl0+ImVec2(dx,-fabs(metrics.fCapHeight)),ImColor(fColCapHeight),2.0f);
            dl->PopClipRect();
        }
    }

    if(ImGui::CollapsingHeader("Paragraph")) {
        for(int i=ImZeroFB::TextAlignFlags_MIN;i<=ImZeroFB::TextAlignFlags_MAX;i++) {
            auto const f = static_cast<ImZeroFB::TextAlignFlags>(i);
            if(ImGui::RadioButton(ImZeroFB::EnumNameTextAlignFlags(f),fTextAlign == f)) {
                fTextAlign = f;
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();

        if(ImGui::TreeNode("English")) {
            ImGui::PushParagraphTextLayout(fTextAlign,ImZeroFB::TextDirection_Ltr);
            ImGui::PushIsParagraphText(ImZeroFB::IsParagraphText_Always);
            ImGui::TextUnformatted("That, poor contempt, or claim'd thou slept so faithful,\n"
                                   "I may contrive our father; and, in their defeated queen,\n"
                                   "Her flesh broke me and puttance of expedition house,\n"
                                   "And in that same that ever I lament this stomach,\n"
                                   "And he, nor Butly and my fury, knowing everything\n"
                                   "Grew daily ever, his great strength and thought\n"
                                   "The bright buds of mine own.\n"
                                   "\n"
                                   "BIONDELLO:\n"
                                   "Marry, that it may not pray their patience.'\n"
                                   "\n"
                                   "KING LEAR:\n"
                                   "The instant common maid, as we may less be\n"
                                   "a brave gentleman and joiner: he that finds us with wax\n"
                                   "And owe so full of presence and our fooder at our\n"
                                   "staves. It is remorsed the bridal's man his grace\n"
                                   "for every business in my tongue, but I was thinking\n"
                                   "that he contends, he hath respected thee.\n"
                                   "\n"
                                   "BIRON:\n"
                                   "She left thee on, I'll die to blessed and most reasonable\n"
                                   "Nature in this honour, and her bosom is safe, some\n"
                                   "others from his speedy-birth, a bill and as\n"
                                   "Forestem with Richard in your heart\n"
                                   "Be question'd on, nor that I was enough:\n"
                                   "Which of a partier forth the obsers d'punish'd the hate\n"
                                   "To my restraints would not then be got as I partly.");
            ImGui::PopIsParagraphText();
            ImGui::PopParagraphTextLayout();
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("German")) {
            ImGui::PushParagraphTextLayout(fTextAlign,ImZeroFB::TextDirection_Ltr);
            ImGui::PushIsParagraphText(ImZeroFB::IsParagraphText_Always);
            ImGui::TextUnformatted("Das Kölner Dombaufest 1848 fand vom 14. bis 16. August 1848 anlässlich des 600. Jahrestages der Grundsteinlegung des Kölner Doms 1248 und der Weihe des provisorisch fertiggestellten Innenraums der Kathedrale statt. Sechs Jahre nach der „zweiten“ Grundsteinlegung zum Weiterbau 1842 war die Grundfläche des Doms zu einem zusammenhängenden, teils noch provisorisch mit einer Holzkonstruktion überdachten Kirchenraum verbunden worden.\n"
                                   "Das von etwa 29.000 Teilnehmern besuchte Fest war als religiöse Feier geplant worden, erhielt durch die Revolutionsereignisse von 1848 aber auch große politische Bedeutung. Sowohl der preußische König Friedrich Wilhelm IV. als auch Reichsverweser Erzherzog Johann von Österreich als höchster Vertreter einer Provisorischen Zentralgewalt der ersten gesamtdeutschen Regierung sowie etwa 300 Abgeordnete der Frankfurter Nationalversammlung, darunter auch deren Präsident Heinrich von Gagern, waren bei den Feierlichkeiten anwesend. Es war damit das einzige größere Zusammentreffen von Repräsentanten der bürgerlichen Revolution und Vertretern der alten Herrschaftsmacht in den deutschen Ländern überhaupt und führte „erst- und letztmalig alle um Einfluss ringenden Parteien an einem Ort zusammen“.");
            ImGui::PopIsParagraphText();
            ImGui::PopParagraphTextLayout();
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Emoji")) {
            ImGui::TextUnformatted("Source: https://perchance.org/emoji");
            ImGui::PushIsParagraphText(ImZeroFB::IsParagraphText_Always);
            ImGui::TextUnformatted(reinterpret_cast<const char*>(u8"🔖 🐶 ⬇️ 📱 🙍 🔵 🛀 ✔️ *⃣ 📤 🐚 🦂 📓 ❌ ⚖ 🎋 📌 🙄 🎳 🈵 🕵 🏯 🐔 📐 🔞 👧 🎊 🐾 💨 🌶 🕕 ✒️ 😰 🏌 🙈 🤘 🔪 ↘️ 🏝 🌌 ⚓️ ♈️ 💾 🖕 😬 💔 🐓 🔟 🔮 🕸 👬 🚆 🎾 🐲 😆 🏥 🍇 🐽 ♏️ 🎷 🍶 🔦 🌄 😿 🌃 🏂 🚈 🙋 🙅 📺 🔠 🎽 👃 💪 💃 💐 ✍ ⛺️ 🏡 📈 🐿 🏊 👱 🍻 ⚡️ 🌴 🐸 📼 🏵 🚁 🔓 🌔 🕖 📷 📕 🕥 👕 🤓 🏖 💒"));
            ImGui::PopIsParagraphText();
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Greek")) {
            ImGui::TextUnformatted("Source: https://lipsum.com");
            ImGui::PushIsParagraphText(ImZeroFB::IsParagraphText_Always);
            ImGui::TextUnformatted(reinterpret_cast<const char*>("Το Lorem Ipsum είναι απλά ένα κείμενο χωρίς νόημα για τους επαγγελματίες της τυπογραφίας και στοιχειοθεσίας. Το Lorem Ipsum είναι το επαγγελματικό πρότυπο όσον αφορά το κείμενο χωρίς νόημα, από τον 15ο αιώνα, όταν ένας ανώνυμος τυπογράφος πήρε ένα δοκίμιο και ανακάτεψε τις λέξεις για να δημιουργήσει ένα δείγμα βιβλίου. Όχι μόνο επιβίωσε πέντε αιώνες, αλλά κυριάρχησε στην ηλεκτρονική στοιχειοθεσία, παραμένοντας με κάθε τρόπο αναλλοίωτο. Έγινε δημοφιλές τη δεκαετία του '60 με την έκδοση των δειγμάτων της Letraset όπου περιελάμβαναν αποσπάσματα του Lorem Ipsum, και πιο πρόσφατα με το λογισμικό ηλεκτρονικής σελιδοποίησης όπως το Aldus PageMaker που περιείχαν εκδοχές του Lorem Ipsum."));
            ImGui::PopIsParagraphText();
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Arabic")) {
            ImGui::TextUnformatted("Source: https://istizada.com/arabic-lorem-ipsum/");
            ImGui::PushIsParagraphText(ImZeroFB::IsParagraphText_Always);
            ImGui::PushParagraphTextLayout(ImZeroFB::TextAlignFlags_Right,ImZeroFB::TextDirection_Rtl);
            ImGui::TextUnformatted(
                    reinterpret_cast<const char *>(u8"لكن لا بد أن أوضح لك أن كل هذه الأفكار المغلوطة حول استنكار  النشوة وتمجيد الألم نشأت بالفعل، وسأعرض لك التفاصيل لتكتشف حقيقة وأساس تلك السعادة البشرية، فلا أحد يرفض أو يكره أو يتجنب الشعور بالسعادة، ولكن بفضل هؤلاء الأشخاص الذين لا يدركون بأن السعادة لا بد أن نستشعرها بصورة أكثر عقلانية ومنطقية فيعرضهم هذا لمواجهة الظروف الأليمة، وأكرر بأنه لا يوجد من يرغب في الحب ونيل المنال ويتلذذ بالآلام، الألم هو الألم ولكن نتيجة لظروف ما قد تكمن السعاده فيما نتحمله من كد وأسي.\n"
                                              "\n"
                                              "و سأعرض مثال حي لهذا، من منا لم يتحمل جهد بدني شاق إلا من أجل الحصول على ميزة أو فائدة؟ ولكن من لديه الحق أن ينتقد شخص ما أراد أن يشعر بالسعادة التي لا تشوبها عواقب أليمة أو آخر أراد أن يتجنب الألم الذي ربما تنجم عنه بعض المتعة ؟ \n"
                                              "علي الجانب الآخر نشجب ونستنكر هؤلاء الرجال المفتونون بنشوة اللحظة الهائمون في رغباتهم فلا يدركون ما يعقبها من الألم والأسي المحتم، واللوم كذلك يشمل هؤلاء الذين أخفقوا في واجباتهم نتيجة لضعف إرادتهم فيتساوي مع هؤلاء الذين يتجنبون وينأون عن تحمل الكدح والألم .\n" ));
            ImGui::PopParagraphTextLayout();
            ImGui::PopIsParagraphText();
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Chinese")) {
            ImGui::TextUnformatted("Source: https://en.wikipedia.org/wiki/Thousand_Character_Classic");
            ImGui::PushIsParagraphText(ImZeroFB::IsParagraphText_Always);
            ImGui::TextUnformatted(reinterpret_cast<const char*>(u8"天地玄黄。"));
            ImGui::PopIsParagraphText();
            ImGui::TreePop();
        }
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

    if(fontMgr != nullptr) {
        if(ImGui::CollapsingHeader("Font Manager")) {
            auto const nFamilies = fontMgr->countFamilies();
            ImGui::TextUnformatted("Families:");
            for(int i=0;i<nFamilies;i++) {
                SkString familyName;
                fontMgr->getFamilyName(i,&familyName);
                if(ImGui::TreeNode(familyName.c_str())) {
                    sk_sp<SkFontStyleSet> set(fontMgr->createStyleSet(i));
                    auto const font = ImGui::skiaFont;
                    for (int j = 0; j < set->count(); ++j) {
                        SkString styleName;
                        SkFontStyle fs;
                        set->getStyle(j, &fs, &styleName);
                        styleName.appendf("%s [%d %d %d]", familyName.c_str(), fs.weight(), fs.width(), fs.slant());
                        ImGui::TextUnformatted(styleName.c_str(), styleName.c_str() + styleName.size());
                        if(ImGui::SmallButton(styleName.c_str())) {
                            ImGui::skiaFont.setTypeface(sk_sp<SkTypeface>(set->createTypeface(j)));
                        }
                    }
                    ImGui::TreePop();
                }
            }
        }
    }
}
