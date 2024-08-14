#include "setupUI.h"
#include <sys/time.h>
#include "imgui_internal.h"
#include "tracy/Tracy.hpp"

#ifdef IMZERO_SKIA_TRACING
#include "src/core/SkATrace.h"
#include "tools/trace/SkDebugfTracer.h"
#include "tools/trace/ChromeTracingTracer.h"
#endif
#include "skiaTracyTracer.h"
#include "implot.h"
#include "buildinfo.gen.h"

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
void ImZeroSkiaSetupUI::render(SaveFormatE &saveFormat, VectorCmdSkiaRenderer &vectorCmdSkiaRenderer, bool &useVectorCmd,
                               size_t totalVectorCmdSerializedSz, size_t totalFffiSz,
                               size_t skpBytes, size_t svgBytes, size_t pngBytes, int windowW, int windowH,
                               SkFontMgr *fontMgr
                               ) { ZoneScoped;
    ImGui::Text("gitCommit=\"%s\",dirty=%s",buildinfo::gitCommit,buildinfo::gitDirty ? "yes" : "no");
    {
        struct timeval tv;
        struct timezone tz;

        gettimeofday(&tv, &tz);

        char buf[sizeof "9999-12-31T23:59:59.999+0000000"];
        size_t bufsize = sizeof buf;
        size_t off = 0;
        struct tm *local = localtime(&tv.tv_sec);
        off = strftime(buf, bufsize, "%FT%T", local); // same as "%Y-%m-%dT%H:%M:%S"
        off += snprintf(buf+off, bufsize-off, ".%06ld", tv.tv_usec);
        off += strftime(buf+off, bufsize-off, "%z", local);

        ImGui::TextUnformatted(buf);
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
    if(saveFormat != SaveFormatE_Disabled) {
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
                saveFormat = SaveFormatE_SVG_TextAsPath;
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

            dl->PushClipRect(p0, p1,true);
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
#ifdef IMZERO_SKIA_TRACING
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
#endif
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
#ifdef IMZERO_SKIA_TRACING
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
#endif
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
        ImGui::TextUnformatted("Latin:");
        for(int i=ImZeroFB::TextAlignFlags_MIN;i<=ImZeroFB::TextAlignFlags_MAX;i++) {
            auto const f = static_cast<ImZeroFB::TextAlignFlags>(i);
            if(ImGui::RadioButton(ImZeroFB::EnumNameTextAlignFlags(f),fTextAlign == f)) {
                fTextAlign = f;
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();

        ImGui::PushParagraphTextLayout(fTextAlign,ImZeroFB::TextDirection_ltr);
        ImGui::PushIsParagraphText(1);
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

        ImGui::TextUnformatted("Emoji:");
        ImGui::PushIsParagraphText(1);
        ImGui::TextUnformatted("🫠👩🏼‍🤝‍👩🏻");
        ImGui::PopIsParagraphText();

        ImGui::TextUnformatted("Arabic https://istizada.com/arabic-lorem-ipsum/:");
        ImGui::PushIsParagraphText(1);
        ImGui::PushParagraphTextLayout(ImZeroFB::TextAlignFlags_right,ImZeroFB::TextDirection_rtl);
        ImGui::TextUnformatted(
                reinterpret_cast<const char *>(u8"لكن لا بد أن أوضح لك أن كل هذه الأفكار المغلوطة حول استنكار  النشوة وتمجيد الألم نشأت بالفعل، وسأعرض لك التفاصيل لتكتشف حقيقة وأساس تلك السعادة البشرية، فلا أحد يرفض أو يكره أو يتجنب الشعور بالسعادة، ولكن بفضل هؤلاء الأشخاص الذين لا يدركون بأن السعادة لا بد أن نستشعرها بصورة أكثر عقلانية ومنطقية فيعرضهم هذا لمواجهة الظروف الأليمة، وأكرر بأنه لا يوجد من يرغب في الحب ونيل المنال ويتلذذ بالآلام، الألم هو الألم ولكن نتيجة لظروف ما قد تكمن السعاده فيما نتحمله من كد وأسي.\n"
                                          "\n"
                                          "و سأعرض مثال حي لهذا، من منا لم يتحمل جهد بدني شاق إلا من أجل الحصول على ميزة أو فائدة؟ ولكن من لديه الحق أن ينتقد شخص ما أراد أن يشعر بالسعادة التي لا تشوبها عواقب أليمة أو آخر أراد أن يتجنب الألم الذي ربما تنجم عنه بعض المتعة ؟ \n"
                                          "علي الجانب الآخر نشجب ونستنكر هؤلاء الرجال المفتونون بنشوة اللحظة الهائمون في رغباتهم فلا يدركون ما يعقبها من الألم والأسي المحتم، واللوم كذلك يشمل هؤلاء الذين أخفقوا في واجباتهم نتيجة لضعف إرادتهم فيتساوي مع هؤلاء الذين يتجنبون وينأون عن تحمل الكدح والألم .\n"
                                          "\t       \u061C"));
        ImGui::PopParagraphTextLayout();
        ImGui::PopIsParagraphText();

        ImGui::TextUnformatted("Chinese https://en.wikipedia.org/wiki/Thousand_Character_Classic:");
        ImGui::PushIsParagraphText(1);
        ImGui::TextUnformatted(reinterpret_cast<const char*>(u8"天地玄黄。"));
        ImGui::PopIsParagraphText();
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
