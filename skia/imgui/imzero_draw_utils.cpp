#include "imzero_draw_utils.h"
#include "imzero_draw_list.h"
#include "imgui_internal.h"
#include "tracy/Tracy.hpp"

inline bool isPasswordFont(const ImFont &font) {
    return font.Glyphs.empty();
}
inline bool isParagraphText(const char *text_begin, const char *text_end) {
    return (memchr(text_begin,'\n',text_end-text_begin) != nullptr);
}

static char hiddenPwBuffer[512];
static size_t hiddenPwBufferNChars = 0;
static size_t hiddenPwBufferNBytesPerChar = 0;
inline void initHiddenPwBuffer(const ImFont &font) {
    if (hiddenPwBufferNChars != 0) {
        return;
    }
    unsigned int cp = ImGui::skiaPasswordDefaultCharacter;
    if (font.FallbackGlyph != nullptr && font.FallbackGlyph->Codepoint > 0) {
        cp = static_cast<unsigned int>(font.FallbackGlyph->Codepoint);
    }
    if (cp < 0x7f) {
        // fast path: single byte character
        hiddenPwBufferNChars = sizeof(hiddenPwBuffer);
        memset(hiddenPwBuffer, static_cast<int>(cp), hiddenPwBufferNChars);
        hiddenPwBufferNBytesPerChar = 1;
    } else {
        // slow path: multibyte character
        ImTextCharToUtf8(hiddenPwBuffer, cp);
        hiddenPwBufferNBytesPerChar = strnlen(hiddenPwBuffer, sizeof(hiddenPwBuffer));
        hiddenPwBufferNChars = sizeof(hiddenPwBufferNChars) / hiddenPwBufferNBytesPerChar;
        for (size_t i = 1; i < hiddenPwBufferNChars; i++) {
            memcpy(&hiddenPwBuffer[i * hiddenPwBufferNBytesPerChar], hiddenPwBuffer,
                   hiddenPwBufferNBytesPerChar);
        }
    }
}
void fbAddPointsToVector(
        flatbuffers::Offset<flatbuffers::Vector<float>> &xs, flatbuffers::Offset<flatbuffers::Vector<float>> &ys,
        flatbuffers::FlatBufferBuilder &builder, const ImVec2 *points, const int points_count) {
    // FIXME use fbBuilder.StartVector and EndVector to eliminate the lambda
    xs = builder.CreateVector<float>(size_t(points_count),[&points](size_t i) -> float { return points[i].x; });
    ys = builder.CreateVector<float>(size_t(points_count),[&points](size_t i) -> float { return points[i].y; });
}
ImZeroFB::DrawListFlags getVectorCmdFBFlags(const ImZeroDrawList &drawList) {
    auto const flags = drawList._delegate->Flags;
    return static_cast<ImZeroFB::DrawListFlags>(
            copyFlag(flags, ImDrawListFlags_AntiAliasedLines, ImZeroFB::DrawListFlags_AntiAliasedLines) |
            copyFlag(flags, ImDrawListFlags_AntiAliasedFill, ImZeroFB::DrawListFlags_AntiAliasedFill));
}
flatbuffers::Offset<ImZeroFB::DrawList> createVectorCmdFBDrawList(ImDrawList drawList, bool inner,
                                                                  std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>> &fbCmds,
                                                                  flatbuffers::FlatBufferBuilder &fbBuilder) { ZoneScoped;
    auto cmds = fbBuilder.CreateVector(fbCmds);
    flatbuffers::Offset<flatbuffers::String> name;
    if(inner) {
        name = 0;
    } else {
        name = fbBuilder.CreateString(drawList._delegate->_OwnerName);
    }
    auto f = getVectorCmdFBFlags(drawList);
    auto const dl = drawList._delegate;

    flatbuffers::Offset<ImZeroFB::VertexData> vertices = 0;
    if(enableVectorCmdFBVertexDraw || !ImGui::useVectorCmd) { ZoneScopedN("de-interleaving vertices");
        flatbuffers::Offset<flatbuffers::Vector<float>> posXYs,texUVs;
        flatbuffers::Offset<flatbuffers::Vector<uint32_t>> cols;
        flatbuffers::Offset<flatbuffers::Vector<uint16_t>> indices;
        auto &vtxBuffer = dl->VtxBuffer;
        auto n = vtxBuffer.size();

        // FIXME use fbBuilder.StartVector and EndVector to eliminate the lambda
        posXYs = fbBuilder.CreateVector<float>(static_cast<size_t>(n)*2,[&vtxBuffer](size_t i) -> float { return i % 2 == 0 ? vtxBuffer[i/2].pos.x : vtxBuffer[i/2].pos.y; });
        texUVs = fbBuilder.CreateVector<float>(static_cast<size_t>(n)*2,[&vtxBuffer](size_t i) -> float { return i % 2 == 0 ? vtxBuffer[i/2].uv.x : vtxBuffer[i/2].uv.y; });
        cols = fbBuilder.CreateVector<uint32_t>(static_cast<size_t>(n),[&vtxBuffer](size_t i) -> uint32_t { return vtxBuffer[i].col; });
        auto &idxBuffer = dl->IdxBuffer;
        indices = fbBuilder.CreateVector<uint16_t>(static_cast<size_t>(idxBuffer.size()),[&idxBuffer](size_t i) -> float { return idxBuffer[i]; });
        vertices = ImZeroFB::CreateVertexData(fbBuilder,posXYs,texUVs,cols,indices);
    } else {
        vertices = 0;
    }
    return ImZeroFB::CreateDrawList(fbBuilder,f,name,vertices,cmds);
}