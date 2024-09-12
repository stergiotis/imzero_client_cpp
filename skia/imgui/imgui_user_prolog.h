#include <stdio.h>
#include <cassert>
#include <cstdlib>

#include "imzero_config.h"
#include "include/core/SkSurface.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkFont.h"
#include "ImZeroFB.out.h"
#include "../skia/paragraph.h"
#include "../skia/imzero_assert.h"

namespace ImGui {
    extern SkFont skiaFont;
    extern bool useVectorCmd;
    extern float skiaFontDyFudge;
    extern std::shared_ptr<Paragraph> paragraph;
    constexpr unsigned int skiaPasswordDefaultCharacter = U'*'; // TODO make this configurable or runtime selectable
// isParagraph: 0 = never, 1 = always, 2 = auto
    void PushIsParagraphText(uint8_t isParagraph);
    void PushParagraphTextLayout(ImZeroFB::TextAlignFlags align,ImZeroFB::TextDirection dir);
    uint8_t PopIsParagraphText();
    void PopParagraphTextLayout();
}

// NOTE: part of ImDrawChannel as std::vector and FlatBufferBuilder are not trivially default constructible
#define IM_DRAW_LIST_SPLITTER_CLASS_EXTRA  ImVector<std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>>*> _ChannelsFbCmds; \
                                           ImVector<flatbuffers::FlatBufferBuilder*> _ChannelsFbBuilders;
#define IM_DRAW_LIST_CLASS_EXTRA           flatbuffers::FlatBufferBuilder *fbBuilder; \
                                           std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>> *_FbCmds; \
                                           uint32_t _FbProcessedDrawCmdIndexOffset; \
                                           void addVectorCmdFB(ImZeroFB::VectorCmdArg arg_type, flatbuffers::Offset<void> arg); \
                                           bool addVerticesAsVectorCmd(); \
                                           ImVector<uint8_t> fPathVerbBuffer; \
                                           ImVector<float> fPathPointBuffer; \
                                           ImVector<float> fPathWeightBuffer; \
                                           void serializeFB(const uint8_t *&out,size_t &sz);


//---- Define assertion handler. Defaults to calling assert().
// If your macro uses multiple statements, make sure is enclosed in a 'do { .. } while (0)' block so it can be used as a single statement.
//#define IM_ASSERT(_EXPR)  MyAssert(_EXPR)
//#define IM_ASSERT(_EXPR)  ((void)(_EXPR))     // Disable asserts
#define IM_ASSERT(_EXPR) ((_EXPR) ? (void)(0) : imzeroAssert(__func__,__FILE__,__LINE__))