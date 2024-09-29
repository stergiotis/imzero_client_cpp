#pragma once
#include <cstdint>
#include "ImZeroFB.out.h"

struct ImDrawList;

namespace ImGui {
    // isParagraph: 0 = never, 1 = always, 2 = auto
    void PushIsParagraphText(ImZeroFB::IsParagraphText isParagraph);
    void PushParagraphTextLayout(ImZeroFB::TextAlignFlags align,ImZeroFB::TextDirection dir);
    uint8_t PopIsParagraphText();
    void PopParagraphTextLayout();
    void DrawSerializedImZeroFB(ImDrawList *drawList,const uint8_t *buf,size_t bufSize);

    extern std::vector<uint8_t> isParagraphTextStack;
    extern std::vector<uint16_t> paragraphTextLayoutStack;
}
