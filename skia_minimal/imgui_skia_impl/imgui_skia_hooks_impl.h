#pragma once

#include "imgui_skia_config.h"
#include "include/core/SkFont.h"
#include "imgui_skia_paragraph.h"

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

namespace ImGui {
    extern SkFont skiaFont;
    extern bool useVectorCmd;
    extern float skiaFontDyFudge;
    extern std::shared_ptr<Paragraph> paragraph;
    constexpr unsigned int skiaPasswordDefaultCharacter = U'*'; // TODO make this configurable or runtime selectable
}
