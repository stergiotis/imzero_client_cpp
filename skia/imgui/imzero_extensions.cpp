#include "imzero_extensions.h"

namespace ImGui {
    std::vector<uint8_t> isParagraphTextStack{};
    std::vector<uint16_t> paragraphTextLayoutStack{};
}

void ImGui::PushIsParagraphText(uint8_t isParagraph) {
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