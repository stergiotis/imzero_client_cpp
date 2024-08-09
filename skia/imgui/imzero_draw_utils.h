#pragma once
#include "imgui.h"
#include "ImZeroFB.out.h"
#include "imzero_draw_list.h"

constexpr bool enableVectorCmdFBVertexDraw = true;

inline bool isPasswordFont(const ImFont &font);
inline bool isParagraphText(const char *text_begin, const char *text_end);
inline void initHiddenPwBuffer(const ImFont &font);
void fbAddPointsToVector(
        flatbuffers::Offset<flatbuffers::Vector<float>> &xs, flatbuffers::Offset<flatbuffers::Vector<float>> &ys,
        flatbuffers::FlatBufferBuilder &builder, const ImVec2 *points, const int points_count);

template<typename T,typename U, typename V>
T copyFlag(V val, U flag1,T flag2) {
    return (static_cast<U>(val) & flag1) != static_cast<U>(0) ? flag2 : static_cast<T>(0);
}
ImZeroFB::DrawListFlags getVectorCmdFBFlags(const ImZeroDrawList &drawList);
flatbuffers::Offset<ImZeroFB::DrawList> createVectorCmdFBDrawList(ImZeroDrawList &drawList,bool inner,
                                                                         std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>> &fbCmds,
                                                                         flatbuffers::FlatBufferBuilder &fbBuilder);