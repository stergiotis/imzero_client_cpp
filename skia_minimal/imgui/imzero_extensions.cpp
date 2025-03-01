#include "imzero_extensions.h"
#include "imgui.h"

namespace ImGui {
    std::vector<uint8_t> isParagraphTextStack{};
    std::vector<uint16_t> paragraphTextLayoutStack{};
    std::vector<uint8_t> textMeasureModeXStack{};
    std::vector<uint8_t> textMeasureModeYStack{};
}

void ImGui::PushIsParagraphText(ImZeroFB::IsParagraphText isParagraph) {
    ImGui::isParagraphTextStack.push_back(isParagraph);
}
uint8_t ImGui::PopIsParagraphText() {
    //IM_ASSERT(!ImGui::isParagraphTextStack.empty() && "unbalanced PushIsParagraphText() and PopIsParagraphText()");
    auto v = ImGui::isParagraphTextStack.back();
    ImGui::isParagraphTextStack.pop_back();
    return v;
}

void ImGui::PushParagraphTextLayout(ImZeroFB::TextAlignFlags align,ImZeroFB::TextDirection dir) {
    ImGui::paragraphTextLayoutStack.push_back((static_cast<uint16_t>(align) << 8) | (static_cast<uint16_t>(dir)));
}
void ImGui::PopParagraphTextLayout() {
    //IM_ASSERT(!ImGui::paragraphTextLayoutStack.empty() && "unbalanced PushParagraphTextLayout() and PopParagraphTextLayout()");
    ImGui::paragraphTextLayoutStack.pop_back();
}
void ImGui::DrawSerializedImZeroFB(ImDrawList *drawList,const uint8_t *buf,size_t bufSize) {
    auto const bufVec = drawList->fbBuilder->CreateVector<uint8_t>(buf,bufSize);
    auto const cmd = ImZeroFB::CreateCmdWrappedDrawList(*drawList->fbBuilder,bufVec);
    drawList->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdWrappedDrawList,cmd.Union());
}
void ImGui::PushTextMeasureMode(ImZeroFB::TextMeasureModeX modeX, ImZeroFB::TextMeasureModeY modeY) {
    ImGui::textMeasureModeXStack.push_back(modeX);
    ImGui::textMeasureModeYStack.push_back(modeY);
}
void ImGui::PopTextMeasureMode() {
    //IM_ASSERT(!ImGui::textMeasureModeXStack.empty() && "unbalanced PushTextMeasureMode() and PopTextMeasureMode()");
    ImGui::textMeasureModeXStack.pop_back();
    ImGui::textMeasureModeYStack.pop_back();
}
