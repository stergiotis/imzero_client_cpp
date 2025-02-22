#include "imgui.h"
#include "tracy/Tracy.hpp"
#include "imgui_internal.h"

#define IMZERO_DRAWLIST_BEGIN if(ImGui::useVectorCmd) {
#define IMZERO_DRAWLIST_END return false; }

static void getParagraphTextLayout(ImZeroFB::TextAlignFlags &align,ImZeroFB::TextDirection &dir) {
    if(ImGui::paragraphTextLayoutStack.empty()) {
        align = ImZeroFB::TextAlignFlags_Left;
        dir = ImZeroFB::TextDirection_Ltr;
    } else {
        auto const v = ImGui::paragraphTextLayoutStack.back();
        align = static_cast<ImZeroFB::TextAlignFlags>(v >> 8);
        dir = static_cast<ImZeroFB::TextDirection>(v & 0xff);
    }
}
static bool isParagraphText(const char *text_begin, const char *text_end) {
    if(!ImGui::isParagraphTextStack.empty()) {
        switch(ImGui::isParagraphTextStack.back()) {
        case ImZeroFB::IsParagraphText_Never: return false;
        case ImZeroFB::IsParagraphText_Always: return true;
        }
    }
    return (memchr(text_begin,'\n',text_end-text_begin) != nullptr);
}
static inline uint64_t castTextureForTransport(ImTextureID textureId) {
    // FIXME textures are currently unused
    //return reinterpret_cast<uint64_t>(reinterpret_cast<intptr_t>(textureId));
    return 0;
}

namespace ImGui {
    SkFont skiaFont;
    bool useVectorCmd = false;
    float skiaFontDyFudge = 0.0f;
    std::shared_ptr<Paragraph> paragraph = nullptr;
}

constexpr bool enableVectorCmdFBVertexDraw = true;

inline bool isPasswordFont(const ImFont &font);
inline void initHiddenPwBuffer(const ImFont &font);
void fbAddPointsToVector(
        flatbuffers::Offset<flatbuffers::Vector<float>> &xs, flatbuffers::Offset<flatbuffers::Vector<float>> &ys,
        flatbuffers::FlatBufferBuilder &builder, const ImVec2 *points, int points_count);

template<typename T,typename U, typename V>
T copyFlag(V val, U flag1,T flag2) {
    return (static_cast<U>(val) & flag1) != static_cast<U>(0) ? flag2 : static_cast<T>(0);
}
ImZeroFB::DrawListFlags getVectorCmdFBFlags(const ImDrawList &drawList);
flatbuffers::Offset<ImZeroFB::DrawList> createVectorCmdFBDrawList(ImDrawList &drawList, bool inner,
                                                                  std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>> &fbCmds,
                                                                  flatbuffers::FlatBufferBuilder &fbBuilder);

inline bool isPasswordFont(const ImFont &font) {
    return font.Glyphs.empty();
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
static bool populatePasswordText(const ::ImFont &font, const char **text_begin, const char **text_end, size_t len) {
    auto freeAllocatedText = false;
    initHiddenPwBuffer(font);
    if(len > hiddenPwBufferNChars) { ZoneScopedN("slow path password text allocation");
        *text_begin = static_cast<char *>(IM_ALLOC(len*hiddenPwBufferNBytesPerChar));
        // slow path, very long or high codepoints password
        for(size_t i=0;i<len;i++) {
            memcpy(const_cast<char*>(&(*text_begin)[i*hiddenPwBufferNBytesPerChar]), hiddenPwBuffer, hiddenPwBufferNBytesPerChar);
        }
        freeAllocatedText = true;
    } else {
        // fast path, password fits in buffer
        (*text_begin) = hiddenPwBuffer;
    }
    (*text_end) = (*text_begin) + len;
    return freeAllocatedText;
}
void fbAddPointsToVector(
        flatbuffers::Offset<flatbuffers::Vector<float>> &xs, flatbuffers::Offset<flatbuffers::Vector<float>> &ys,
        flatbuffers::FlatBufferBuilder &builder, const ImVec2 *points, const int points_count) {
    // FIXME use fbBuilder.StartVector and EndVector to eliminate the lambda
    xs = builder.CreateVector<float>(static_cast<size_t>(points_count),[&points](const size_t i) -> float { return points[i].x; });
    ys = builder.CreateVector<float>(static_cast<size_t>(points_count),[&points](const size_t i) -> float { return points[i].y; });
}
bool ImDrawList::addVerticesAsVectorCmd() {
    const auto sz = CmdBuffer.Size;
    if(sz >= 2) {
        _TryMergeDrawCmds();
    }
    bool added = false;
    for(int i=0;i<sz;i++) {
        auto &cur = CmdBuffer.Data[i];
        if(cur.ElemCount == 0 || cur.UserCallback != nullptr) {
            continue;
        }
        auto cr = ImZeroFB::SingleVec4(cur.ClipRect.x,cur.ClipRect.y,cur.ClipRect.z,cur.ClipRect.w);
        auto cmd = ImZeroFB::CreateCmdVertexDraw(*fbBuilder,
                                                 &cr,
                                                 cur.ElemCount,
                                                 _FbProcessedDrawCmdIndexOffset,
                                                 cur.VtxOffset);
        _FbCmds->push_back(ImZeroFB::CreateSingleVectorCmdDto(*fbBuilder,ImZeroFB::VectorCmdArg_CmdVertexDraw,cmd.Union()));
        _FbProcessedDrawCmdIndexOffset += cur.ElemCount;
        added = true;
    }
    return added;
}
void ImDrawList::serializeFB(const uint8_t *&out,size_t &size) { ZoneScoped;
    if(!ImGui::useVectorCmd) {
        // no native drawing commands, add all vertices as command (this will emulate the standard ImGui backend implementation)
        addVerticesAsVectorCmd();
    }

    auto dlFb = createVectorCmdFBDrawList(*this,false,*_FbCmds,*fbBuilder);
    fbBuilder->FinishSizePrefixed(dlFb,nullptr);
    size = fbBuilder->GetSize();
    out = fbBuilder->GetBufferPointer();
}

ImZeroFB::DrawListFlags getVectorCmdFBFlags(const ImDrawList &drawList) {
    auto const flags = drawList.Flags;
    return static_cast<ImZeroFB::DrawListFlags>(
            copyFlag(flags, ImDrawListFlags_AntiAliasedLines, ImZeroFB::DrawListFlags_AntiAliasedLines) |
            copyFlag(flags, ImDrawListFlags_AntiAliasedFill, ImZeroFB::DrawListFlags_AntiAliasedFill));
}
flatbuffers::Offset<ImZeroFB::DrawList> createVectorCmdFBDrawList(ImDrawList &drawList, bool inner,
                                                                  std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>> &fbCmds,
                                                                  flatbuffers::FlatBufferBuilder &fbBuilder) { ZoneScoped;
    auto cmds = fbBuilder.CreateVector(fbCmds);
    flatbuffers::Offset<flatbuffers::String> name;
    if(inner) {
        name = 0;
    } else {
        name = fbBuilder.CreateString(drawList._OwnerName);
    }
    auto f = getVectorCmdFBFlags(drawList);

    flatbuffers::Offset<ImZeroFB::VertexData> vertices = 0;
    if(enableVectorCmdFBVertexDraw || !ImGui::useVectorCmd) { ZoneScopedN("de-interleaving vertices");
        flatbuffers::Offset<flatbuffers::Vector<float>> posXYs,texUVs;
        flatbuffers::Offset<flatbuffers::Vector<uint32_t>> cols;
        flatbuffers::Offset<flatbuffers::Vector<uint16_t>> indices;
        auto &vtxBuffer = drawList.VtxBuffer;
        auto n = vtxBuffer.size();

        // FIXME use fbBuilder.StartVector and EndVector to eliminate the lambda
        posXYs = fbBuilder.CreateVector<float>(static_cast<size_t>(n)*2,[&vtxBuffer](size_t i) -> float { return i % 2 == 0 ? vtxBuffer[i/2].pos.x : vtxBuffer[i/2].pos.y; });
        texUVs = fbBuilder.CreateVector<float>(static_cast<size_t>(n)*2,[&vtxBuffer](size_t i) -> float { return i % 2 == 0 ? vtxBuffer[i/2].uv.x : vtxBuffer[i/2].uv.y; });
        cols = fbBuilder.CreateVector<uint32_t>(static_cast<size_t>(n),[&vtxBuffer](size_t i) -> uint32_t { return vtxBuffer[i].col; });
        auto &idxBuffer = drawList.IdxBuffer;
        indices = fbBuilder.CreateVector<uint16_t>(static_cast<size_t>(idxBuffer.size()),[&idxBuffer](size_t i) -> float { return idxBuffer[i]; });
        vertices = ImZeroFB::CreateVertexData(fbBuilder,posXYs,texUVs,cols,indices);
    }
    return ImZeroFB::CreateDrawList(fbBuilder,f,name,vertices,cmds);
}

void ImDrawList::addVectorCmdFB(ImZeroFB::VectorCmdArg arg_type, flatbuffers::Offset<void> arg) {
    if(enableVectorCmdFBVertexDraw && ImGui::useVectorCmd) {
        if(addVerticesAsVectorCmd()) {
            this->CmdBuffer.clear();
            AddDrawCmd();
        }
    }
    //fprintf(stderr,"%s: adding %s\n", _OwnerName, ImZeroFB::EnumNameVectorCmdArg(arg_type));
    _FbCmds->push_back(ImZeroFB::CreateSingleVectorCmdDto(*fbBuilder,arg_type,arg));
}


namespace ImGui {
    void Hooks::Global::ShouldAddDrawListToDrawData(const ::ImDrawList *draw_list, bool &shouldAdd) {
        shouldAdd = shouldAdd || !draw_list->_FbCmds->empty();
    }
    bool Hooks::Global::Pre::RenderDimmedBackdgroundBehindWindow(::ImGuiWindow *window, ImU32 col) {
        return false;
    }
    bool Hooks::Global::Pre::InputTextCalcTextSize(::ImVec2 &out, ::ImGuiContext* ctx, const char* text_begin, const char* text_end, const char** remaining, ImVec2* out_offset, bool stop_on_new_line) {
        if (!ImGui::useVectorCmd) {
           return true;
        }

        const ::ImGuiContext& g = *ctx;
        ::ImFont *font = g.Font;
        out = ImVec2(0, 0);

        if (stop_on_new_line) { // TODO use skia paragraph max line feature
            auto const lineEnd =  static_cast<const char*>(memchr(text_begin, '\n', text_end - text_begin));
            if (lineEnd != nullptr) {
                text_end = lineEnd-1;
                if (remaining) {
                    *remaining = lineEnd;
                }
            }
        }

        const auto wrap_width = ImGui::GetContentRegionAvail().x;
        if(wrap_width <= 0.0) {
            return false;
        }

        auto freeAllocatedText = false;
        if(isPasswordFont(*font)) {
            // assumes passwords are rendered on a single line
            freeAllocatedText = populatePasswordText(*font, &text_begin, &text_end, static_cast<size_t>(text_end-text_begin));
        }

#if 0
        paragraph->disableFontFallback();
        paragraph->setFontSize(SkFloatToScalar(g.FontSize));
        paragraph->build(text_begin,static_cast<size_t>(text_end-text_begin));
        paragraph->layout(SkScalarToFloat(wrap_width));
        paragraph->enableFontFallback();
        out.x = SkScalarToFloat(paragraph->getMaxIntrinsicWidth());
        out.y = SkScalarToFloat(paragraph->getHeight());
#else
        const auto f = ImGui::skiaFont.makeWithSize(SkFloatToScalar(g.FontSize));
        const SkScalar advanceWidth = f.measureText(text_begin,text_end-text_begin,SkTextEncoding::kUTF8, nullptr);
        out.x = advanceWidth;
        out.y = g.FontSize;
#endif

        if (out_offset) {
            const auto nLines = paragraph->getNumberOfLines();
            bool found;
            const auto bb = paragraph->boundingRect(nLines-1,found);
            IM_ASSERT(!found && "last line not found in paragraph");
            auto line_height = out.y / static_cast<float>(nLines);
            *out_offset = ImVec2(bb.width(), out.y + line_height);
        }

        if (freeAllocatedText) {
            IM_FREE(const_cast<char*>(text_begin));
        }

        return false;
    }

    void Hooks::ImDrawListSplitter::SaveDrawListToSplitter(::ImDrawListSplitter &splitter, const ::ImDrawList *draw_list, int idx) {
        IM_ASSERT(splitter._ChannelsFbCmds[idx] != nullptr && "uninitialized channel fb");
        splitter._ChannelsFbCmds[idx] = draw_list->_FbCmds;
        splitter._ChannelsFbBuilders[idx] = draw_list->fbBuilder;
    }
    void Hooks::ImDrawListSplitter::RestoreDrawListFromSplitter(::ImDrawListSplitter const &splitter, ::ImDrawList *draw_list, int idx) {
        IM_ASSERT(splitter._ChannelsFbCmds[idx] != nullptr && "uninitialized channel fb");
        draw_list->_FbCmds = splitter._ChannelsFbCmds[idx];
        draw_list->fbBuilder = splitter._ChannelsFbBuilders[idx];
    }
    int Hooks::ImDrawListSplitter::EnsureSlotsCapacity(::ImDrawListSplitter &splitter, int nSlotsTotal) {
        // _Current is zero when ImDrawListSplitter gets initialized but _Channels.Data[0] is not properly
        // initialized. Why does the regular ImGui code get away with the check below?
        auto b = splitter._ChannelsFbCmds.size();
        if(b < nSlotsTotal) { ZoneScopedN("initialize additional channels");
            splitter._ChannelsFbCmds.reserve(nSlotsTotal);
            splitter._ChannelsFbCmds.resize(nSlotsTotal);
            splitter._ChannelsFbBuilders.reserve(nSlotsTotal);
            splitter._ChannelsFbBuilders.resize(nSlotsTotal);
            return nSlotsTotal-b;
        } else {
            return 0;
        }
    }
    void Hooks::ImDrawListSplitter::ResetSlot(::ImDrawListSplitter &splitter,int i) {
        splitter._ChannelsFbCmds[i]->resize(0);
        splitter._ChannelsFbBuilders[i]->Clear();
    }
    void Hooks::ImDrawListSplitter::InitSlot(::ImDrawListSplitter &splitter,int i) {
        splitter._ChannelsFbCmds[i] = IM_NEW(std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>>);
        splitter._ChannelsFbBuilders[i] = IM_NEW(flatbuffers::FlatBufferBuilder);
    }
    void Hooks::ImDrawListSplitter::InitSlots(::ImDrawListSplitter &splitter, int nSlotsToInit) {
        auto b = splitter._ChannelsFbCmds.size();
        IM_ASSERT(nSlotsToInit <= b && nSlotsToInit >= 0 && "nSlotsToInit is out of bounds" );
        for(auto i=b-nSlotsToInit;i<b;i++) {
            Hooks::ImDrawListSplitter::InitSlot(splitter,i);
        }
    }
    void Hooks::ImDrawListSplitter::ClearFreeMemory(::ImDrawListSplitter &splitter) {
        for (int i = 0; i < splitter._ChannelsFbCmds.Size; i++)
        {
            if (i == splitter._Current) {
                splitter._ChannelsFbCmds[i] = nullptr;
                splitter._ChannelsFbBuilders[i] = nullptr;
            } else {
                splitter._ChannelsFbCmds[i]->clear();
                IM_DELETE(splitter._ChannelsFbCmds[i]);
                splitter._ChannelsFbBuilders[i]->Clear();
                IM_DELETE(splitter._ChannelsFbBuilders[i]);
            }
        }
        splitter._ChannelsFbCmds.clear();
        splitter._ChannelsFbBuilders.clear();
    }
    int Hooks::ImDrawListSplitter::MergeInitialValue(::ImDrawListSplitter &splitter,::ImDrawList *drawList) {
        IM_ASSERT_PARANOID(drawList->fbBuilder == splitter._ChannelsFbBuilders[0] && "lowest channel is active channel");
        IM_ASSERT_PARANOID(drawList->_FbCmds == splitter._ChannelsFbCmds[0] && "lowest channel is active channel");
        return 0;
    }
    int Hooks::ImDrawListSplitter::MergeUpdate(::ImDrawListSplitter &splitter,int i,int prev) {
        return prev + static_cast<int>(splitter._ChannelsFbCmds[i]->size());
    }
    void Hooks::ImDrawListSplitter::MergeReserve(::ImDrawList *drawList,int n) {
        drawList->_FbCmds->reserve(drawList->_FbCmds->size() + n);
    }
    void Hooks::ImDrawListSplitter::MergeOp(::ImDrawListSplitter &splitter,::ImDrawList *drawList,int i) { ZoneScopedN("serialize and add split draw list");
        // build command
        auto builder = splitter._ChannelsFbBuilders[i];
        auto dlFb = createVectorCmdFBDrawList(*drawList,true,*splitter._ChannelsFbCmds[i],*builder);
        builder->Finish(dlFb,nullptr);

        // serialize
        auto const buf = builder->GetBufferPointer();
        auto const bufSize = builder->GetSize();

        // append to drawlist
        auto const bufVec = drawList->fbBuilder->CreateVector<uint8_t>(buf,bufSize);
        auto const cmd = ImZeroFB::CreateCmdWrappedDrawList(*drawList->fbBuilder,bufVec);
        drawList->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdWrappedDrawList,cmd.Union());

        builder->Clear();
    }


    bool Hooks::ImDrawList::Pre::_ResetForNewFrame(::ImDrawList *draw_list) { ZoneScoped;
        draw_list->_FbProcessedDrawCmdIndexOffset = 0;
        if(draw_list->fbBuilder == nullptr) {
            draw_list->fbBuilder = new flatbuffers::FlatBufferBuilder();
        } else {
            draw_list->fbBuilder->Clear();
        }
        if(draw_list->_FbCmds == nullptr) {
            draw_list->_FbCmds = new std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>>();
        } else {
            draw_list->_FbCmds->resize(0);
        }
        return true;
    }

    bool Hooks::ImDrawList::Pre::_ClearFreeMemory(::ImDrawList *draw_list) { ZoneScoped;
        if(draw_list->fbBuilder != nullptr) {
            draw_list->fbBuilder->Reset();
            delete draw_list->fbBuilder;
            draw_list->fbBuilder = nullptr;
        }
        if(draw_list->_FbCmds != nullptr) {
            draw_list->_FbCmds->clear();
            delete draw_list->_FbCmds;
            draw_list->_FbCmds = nullptr;
        }
#ifdef IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
        draw_list->fPathVerbBuffer.clear();
        draw_list->fPathPointBuffer.clear();
        draw_list->fPathWeightBuffer.clear();
#endif
        return true;
    }

    bool Hooks::ImDrawList::Pre::CloneOutput(::ImDrawList *draw_list,::ImDrawList *dst) { ZoneScoped;
        dst->fbBuilder = draw_list->fbBuilder;
        dst->_FbCmds = draw_list->_FbCmds;
#ifdef IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
        dst->fPathVerbBuffer = draw_list->fPathVerbBuffer;
        dst->fPathPointBuffer = draw_list->fPathPointBuffer;
        dst->fPathWeightBuffer = draw_list->fPathWeightBuffer;
#endif
        return true;
    }

    bool Hooks::ImDrawList::Pre::_PopUnusedDrawCmd(::ImDrawList *draw_list) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::_TryMergeDrawCmds(::ImDrawList *draw_list) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::_OnChangedClipRect(::ImDrawList *draw_list) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::_OnChangedTextureID(::ImDrawList *draw_list) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::_OnChangedVtxOffset(::ImDrawList *draw_list) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PushClipRect(::ImDrawList *draw_list, const ::ImVec2& cr_min, const ::ImVec2& cr_max, bool intersect_with_current_clip_rect) { ZoneScoped;
        return true;
    }
    void Hooks::ImDrawList::Post::PushClipRect(::ImDrawList *draw_list, const ::ImVec2& cr_min, const ::ImVec2& cr_max, bool intersect_with_current_clip_rect) { ZoneScoped;
        auto cr = draw_list->_CmdHeader.ClipRect ;
        auto rect = ImZeroFB::SingleVec4(cr.x,cr.y,cr.z,cr.w); // NOTE: use of intersected rectangle is mandatory
        auto arg = ImZeroFB::CreateCmdPushClipRect(*draw_list->fbBuilder,&rect,intersect_with_current_clip_rect);
        draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdPushClipRect,arg.Union());
    }

    bool Hooks::ImDrawList::Pre::PushClipRectFullScreen(::ImDrawList *draw_list) { ZoneScoped;
        return true;
    }

    void Hooks::ImDrawList::Post::PopClipRect(::ImDrawList *draw_list) { ZoneScoped;
        if(draw_list->fbBuilder != nullptr) {
            auto arg = ImZeroFB::CreateCmdPopClipRect(*draw_list->fbBuilder);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdPopClipRect,arg.Union());
        }
    }
    void Hooks::ImDrawList::Post::CloneOutput(const ::ImDrawList *draw_list, ::ImDrawList *dest) {
        // FIXME
    }
    void Hooks::ImDrawList::Post::CreateImDrawList(::ImDrawList *draw_list, ImDrawListSharedData *shared_data) {
        ;
    }
    bool Hooks::ImDrawList::Pre::AddCallback(::ImDrawList *draw_list, void *callback, void *userdata, size_t userdata_size) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::AddDrawCmd(::ImDrawList *draw_list) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::PathClear(::ImDrawList *draw_list) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::PathFillConvex(::ImDrawList *draw_list, ImU32 col) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::PathFillConcave(::ImDrawList *draw_list, ImU32 col) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::PathLineTo(::ImDrawList *draw_list, const ImVec2 &pos) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::PathStroke(::ImDrawList *draw_list, ImU32 col, ImDrawFlags flags, float thickness) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::PopClipRect(::ImDrawList *draw_list) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::PrimVtx(::ImDrawList *draw_list, const ImVec2 &pos, const ImVec2 &uv, ImU32 col) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::PrimWriteIdx(::ImDrawList *draw_list, ImDrawIdx idx) {
        return true;
    }
    bool Hooks::ImDrawList::Pre::PrimWriteVtx(::ImDrawList *draw_list, const ImVec2 &pos, const ImVec2 &uv, ImU32 col) {
        return true;
    }

    bool Hooks::ImDrawList::Pre::PushTextureID(::ImDrawList *draw_list, ImTextureID texture_id) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PopTextureID(::ImDrawList *draw_list) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PrimReserve(::ImDrawList *draw_list, int idx_count, int vtx_count) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PrimUnreserve(::ImDrawList *draw_list, int idx_count, int vtx_count) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PrimRect(::ImDrawList *draw_list, const ImVec2& a, const ImVec2& c, ImU32 col) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PrimRectUV(::ImDrawList *draw_List,const ImVec2& a, const ImVec2& c, const ImVec2& uv_a, const ImVec2& uv_c, ImU32 col) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PrimQuadUV(::ImDrawList *draw_list, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col) { ZoneScoped;
        return true;
    }


    bool Hooks::ImDrawList::Pre::AddPolyline(::ImDrawList *draw_list, const ImVec2* points, const int points_count, ImU32 col, ImDrawFlags flags, float thickness) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            {
                flatbuffers::Offset<flatbuffers::Vector<float>> xs, ys;
                fbAddPointsToVector(xs,ys,*draw_list->fbBuilder,points,points_count);
                auto p = ImZeroFB::CreateArrayOfVec2(*draw_list->fbBuilder,xs,ys);
                auto arg = ImZeroFB::CreateCmdPolyline(*draw_list->fbBuilder,p,col,flags,thickness);
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdPolyline,arg.Union());
                return false;
            }
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddConvexPolyFilled(::ImDrawList *draw_list,const ImVec2* points, const int points_count, ImU32 col) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            flatbuffers::Offset<flatbuffers::Vector<float>> xs, ys;
            fbAddPointsToVector(xs,ys,*draw_list->fbBuilder,points,points_count);
            auto p = ImZeroFB::CreateArrayOfVec2(*draw_list->fbBuilder,xs,ys);
            auto arg = ImZeroFB::CreateCmdConvexPolyFilled(*draw_list->fbBuilder,p,col);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdConvexPolyFilled,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }
    bool Hooks::ImDrawList::Pre::AddConcavePolyFilled(::ImDrawList *draw_list, const ImVec2 *points, int num_points, ImU32 col) {
        ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            flatbuffers::Offset<flatbuffers::Vector<float>> xs, ys;
            fbAddPointsToVector(xs,ys,*draw_list->fbBuilder,points,num_points);
            auto p = ImZeroFB::CreateArrayOfVec2(*draw_list->fbBuilder,xs,ys);
            auto arg = ImZeroFB::CreateCmdConcavePolyFilled(*draw_list->fbBuilder,p,col);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdConcavePolyFilled,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }


    bool Hooks::ImDrawList::Pre::PathArcToFast(::ImDrawList *draw_list, const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PathArcTo(::ImDrawList *draw_list, const ImVec2& center, float radius, float a_min, float a_max, int num_segments) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PathEllipticalArcTo(::ImDrawList *draw_list, const ImVec2& center, const ImVec2 &radius, float rot, float a_min, float a_max, int num_segments) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PathBezierCubicCurveTo(::ImDrawList *draw_list, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::PathBezierQuadraticCurveTo(::ImDrawList *draw_list, const ImVec2& p2, const ImVec2& p3, int num_segments) { ZoneScoped;
        return true;
    }

    static inline ImDrawFlags FixRectCornerFlags(ImDrawFlags flags)
    { ZoneScoped;
        /*
    IM_STATIC_ASSERT(ImDrawFlags_RoundCornersTopLeft == (1 << 4));
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    // Obsoleted in 1.82 (from February 2021). This code was stripped/simplified and mostly commented in 1.90 (from September 2023)
    // - Legacy Support for hard coded ~0 (used to be a suggested equivalent to ImDrawCornerFlags_All)
    if (flags == ~0)                    { return ImDrawFlags_RoundCornersAll; }
    // - Legacy Support for hard coded 0x01 to 0x0F (matching 15 out of 16 old flags combinations). Read details in older version of this code.
    if (flags >= 0x01 && flags <= 0x0F) { return (flags << 4); }
    // We cannot support hard coded 0x00 with 'float rounding > 0.0f' --> replace with ImDrawFlags_RoundCornersNone or use 'float rounding = 0.0f'
#endif
    */
        // If this assert triggers, please update your code replacing hardcoded values with new ImDrawFlags_RoundCorners* values.
        // Note that ImDrawFlags_Closed (== 0x01) is an invalid flag for AddRect(), AddRectFilled(), PathRect() etc. anyway.
        // See details in 1.82 Changelog as well as 2021/03/12 and 2023/09/08 entries in "API BREAKING CHANGES" section.
        IM_ASSERT((flags & 0x0F) == 0 && "Misuse of legacy hardcoded ImDrawCornerFlags values!");

        if ((flags & ImDrawFlags_RoundCornersMask_) == 0)
            flags |= ImDrawFlags_RoundCornersAll;

        return flags;
    }

    bool Hooks::ImDrawList::Pre::PathRect(::ImDrawList *draw_list, const ImVec2& a, const ImVec2& b, float rounding, ImDrawFlags flags) { ZoneScoped;
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddLine(::ImDrawList *draw_list, const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
            auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
            auto arg = ImZeroFB::CreateCmdLine(*draw_list->fbBuilder,&p1Fb,&p2Fb,col,thickness);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdLine,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddRect(::ImDrawList *draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags, float thickness) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
            auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
            if (rounding >= 0.5f) {
                flags = FixRectCornerFlags(flags);
                rounding = ImMin(rounding, ImFabs(p_max.x - p_min.x) * (((flags & ImDrawFlags_RoundCornersTop) == ImDrawFlags_RoundCornersTop) || ((flags & ImDrawFlags_RoundCornersBottom) == ImDrawFlags_RoundCornersBottom) ? 0.5f : 1.0f) - 1.0f);
                rounding = ImMin(rounding, ImFabs(p_max.y - p_min.y) * (((flags & ImDrawFlags_RoundCornersLeft) == ImDrawFlags_RoundCornersLeft) || ((flags & ImDrawFlags_RoundCornersRight) == ImDrawFlags_RoundCornersRight) ? 0.5f : 1.0f) - 1.0f);
            }
            if (rounding < 0.5f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersNone) {
                rounding = 0.0f;
            }

            if(rounding == 0.0f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersAll) {
                auto arg = ImZeroFB::CreateCmdRectRounded(*draw_list->fbBuilder,&pMinFb,&pMaxFb,col,rounding,thickness);
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectRounded,arg.Union());
            } else {
                const float rounding_tl = (flags & ImDrawFlags_RoundCornersTopLeft)     ? rounding : 0.0f;
                const float rounding_tr = (flags & ImDrawFlags_RoundCornersTopRight)    ? rounding : 0.0f;
                const float rounding_br = (flags & ImDrawFlags_RoundCornersBottomRight) ? rounding : 0.0f;
                const float rounding_bl = (flags & ImDrawFlags_RoundCornersBottomLeft)  ? rounding : 0.0f;
                auto arg = ImZeroFB::CreateCmdRectRoundedCorners(*draw_list->fbBuilder,&pMinFb,&pMaxFb,col,rounding_tl,rounding_tr,rounding_br,rounding_bl,thickness);
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectRoundedCorners,arg.Union());
            }
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddRectFilled(::ImDrawList *draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
            auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
            if (rounding >= 0.5f) {
                flags = FixRectCornerFlags(flags);
                rounding = ImMin(rounding, ImFabs(p_max.x - p_min.x) * (((flags & ImDrawFlags_RoundCornersTop) == ImDrawFlags_RoundCornersTop) || ((flags & ImDrawFlags_RoundCornersBottom) == ImDrawFlags_RoundCornersBottom) ? 0.5f : 1.0f) - 1.0f);
                rounding = ImMin(rounding, ImFabs(p_max.y - p_min.y) * (((flags & ImDrawFlags_RoundCornersLeft) == ImDrawFlags_RoundCornersLeft) || ((flags & ImDrawFlags_RoundCornersRight) == ImDrawFlags_RoundCornersRight) ? 0.5f : 1.0f) - 1.0f);
            }

            if (rounding < 0.5f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersNone) {
                rounding = 0.0f;
            }

            if(rounding == 0.0f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersAll) {
                auto arg = ImZeroFB::CreateCmdRectRoundedFilled(*draw_list->fbBuilder,&pMinFb,&pMaxFb,col,rounding);
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectRoundedFilled,arg.Union());
            } else {
                const float rounding_tl = (flags & ImDrawFlags_RoundCornersTopLeft)     ? rounding : 0.0f;
                const float rounding_tr = (flags & ImDrawFlags_RoundCornersTopRight)    ? rounding : 0.0f;
                const float rounding_br = (flags & ImDrawFlags_RoundCornersBottomRight) ? rounding : 0.0f;
                const float rounding_bl = (flags & ImDrawFlags_RoundCornersBottomLeft)  ? rounding : 0.0f;
                auto arg = ImZeroFB::CreateCmdRectRoundedCornersFilled(*draw_list->fbBuilder,&pMinFb,&pMaxFb,col,rounding_tl,rounding_tr,rounding_br,rounding_bl);
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectRoundedCornersFilled,arg.Union());
            }
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddRectFilledMultiColor(::ImDrawList *draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            return true;
            if(false){ // FIXME
                auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
                auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
                auto arg = ImZeroFB::CreateCmdRectFilledMultiColor(*draw_list->fbBuilder,&pMinFb,&pMaxFb,col_upr_left,col_upr_right,col_bot_right,col_bot_left);
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectFilledMultiColor,arg.Union());
            }
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddQuad(::ImDrawList *draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
            auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
            auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
            auto p4Fb = ImZeroFB::SingleVec2(p4.x,p4.y);
            auto arg = ImZeroFB::CreateCmdQuad(*draw_list->fbBuilder,&p1Fb,&p2Fb,&p3Fb,&p4Fb,col,thickness);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdQuad,arg.Union());
        IMZERO_DRAWLIST_END

        return true;
    }

    bool Hooks::ImDrawList::Pre::AddQuadFilled(::ImDrawList *draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
            auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
            auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
            auto p4Fb = ImZeroFB::SingleVec2(p4.x,p4.y);
            auto arg = ImZeroFB::CreateCmdQuadFilled(*draw_list->fbBuilder,&p1Fb,&p2Fb,&p3Fb,&p4Fb,col);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdQuadFilled,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddTriangle(::ImDrawList *draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
            auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
            auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
            auto arg = ImZeroFB::CreateCmdTriangle(*draw_list->fbBuilder,&p1Fb,&p2Fb,&p3Fb,col);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdTriangle,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddTriangleFilled(::ImDrawList *draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
            auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
            auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
            auto arg = ImZeroFB::CreateCmdTriangleFilled(*draw_list->fbBuilder,&p1Fb,&p2Fb,&p3Fb,col);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdTriangleFilled,arg.Union());
        IMZERO_DRAWLIST_END

        return true;
    }

    bool Hooks::ImDrawList::Pre::AddCircle(::ImDrawList *draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
            auto arg = ImZeroFB::CreateCmdCircle(*draw_list->fbBuilder,&centerFb,radius,col,num_segments,thickness);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdCircle,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddCircleFilled(::ImDrawList *draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
            auto arg = ImZeroFB::CreateCmdCircleFilled(*draw_list->fbBuilder,&centerFb,radius,col);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdCircleFilled,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddNgon(::ImDrawList *draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
            auto arg = ImZeroFB::CreateCmdNgon(*draw_list->fbBuilder,&centerFb,radius,col,num_segments,thickness);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdNgon,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddNgonFilled(::ImDrawList *draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
            auto arg = ImZeroFB::CreateCmdNgonFilled(*draw_list->fbBuilder,&centerFb,radius,col,num_segments);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdNgonFilled,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddEllipse(::ImDrawList *draw_list, const ImVec2& center, const ImVec2& radius, ImU32 col, float rot, int num_segments, float thickness) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
            auto radiusFb = ImZeroFB::SingleVec2(radius.x,radius.y);
            auto arg = ImZeroFB::CreateCmdEllipse(*draw_list->fbBuilder,&centerFb,&radiusFb,col,rot,num_segments,thickness);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdEllipse,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddEllipseFilled(::ImDrawList *draw_list, const ImVec2& center, const ImVec2& radius, ImU32 col, float rot, int num_segments) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
            auto radiusFb = ImZeroFB::SingleVec2(radius.x,radius.y);
            auto arg = ImZeroFB::CreateCmdEllipseFilled(*draw_list->fbBuilder,&centerFb,&radiusFb,col,rot,num_segments);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdEllipseFilled,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddBezierCubic(::ImDrawList *draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
            auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
            auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
            auto p4Fb = ImZeroFB::SingleVec2(p4.x,p4.y);
            auto arg = ImZeroFB::CreateCmdBezierCubic(*draw_list->fbBuilder,&p1Fb,&p2Fb,&p3Fb,&p4Fb,col,thickness,num_segments);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdBezierCubic,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddBezierQuadratic(::ImDrawList *draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness, int num_segments) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
            auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
            auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
            auto arg = ImZeroFB::CreateCmdBezierQuadratic(*draw_list->fbBuilder,&p1Fb,&p2Fb,&p3Fb,col,thickness,num_segments);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdBezierQuadratic,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddImage(::ImDrawList *draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
            auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
            auto uvMinFb = ImZeroFB::SingleVec2(uv_min.x,uv_min.y);
            auto uvMaxFb = ImZeroFB::SingleVec2(uv_max.x,uv_max.y);
            auto arg = ImZeroFB::CreateCmdImage(*draw_list->fbBuilder,castTextureForTransport(user_texture_id),&pMinFb,&pMaxFb,&uvMinFb,&uvMaxFb,col);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdImage,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddImageQuad(::ImDrawList *draw_list, ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1, const ImVec2& uv2, const ImVec2& uv3, const ImVec2& uv4, ImU32 col) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
            auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
            auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
            auto p4Fb = ImZeroFB::SingleVec2(p4.x,p4.y);
            auto uv1Fb = ImZeroFB::SingleVec2(uv1.x,uv1.y);
            auto uv2Fb = ImZeroFB::SingleVec2(uv2.x,uv2.y);
            auto uv3Fb = ImZeroFB::SingleVec2(uv3.x,uv3.y);
            auto uv4Fb = ImZeroFB::SingleVec2(uv4.x,uv4.y);
            auto arg = ImZeroFB::CreateCmdImageQuad(*draw_list->fbBuilder,castTextureForTransport(user_texture_id),&p1Fb,&p2Fb,&p3Fb,&p4Fb,&uv1Fb,&uv2Fb,&uv3Fb,&uv4Fb,col);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdImageQuad,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImDrawList::Pre::AddImageRounded(::ImDrawList *draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawFlags flags) { ZoneScoped;
        IMZERO_DRAWLIST_BEGIN
            auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
            auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
            auto uvMinFb = ImZeroFB::SingleVec2(uv_min.x,uv_max.y);
            auto uvMaxFb = ImZeroFB::SingleVec2(uv_min.x,uv_max.y);
            auto arg = ImZeroFB::CreateCmdImageRounded(*draw_list->fbBuilder,castTextureForTransport(user_texture_id),&pMinFb,&pMaxFb,&uvMinFb,&uvMaxFb,col,rounding,flags);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdImageRounded,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImFont::Pre::GetCharAdvance(const ::ImFont *font, float &retr, ImWchar c) { ZoneScoped;
        if(!ImGui::useVectorCmd) {
            return true;
        }
        IM_ASSERT(font != nullptr && "font is nullptr");

        uint32_t tmp;
        if(font->Glyphs.empty()) {
            // password font
            tmp = ImGui::skiaPasswordDefaultCharacter;
        } else {
            tmp = c;
        }
        auto const f = ImGui::skiaFont.makeWithSize(SkFloatToScalar(font->FontSize));
        auto const glyph = f.unicharToGlyph(SkUnichar(tmp));
        SkScalar advanceX;
        f.getWidths(&glyph,1,&advanceX);
        retr = SkScalarToFloat(advanceX);
        return false;
    }

    bool Hooks::ImFont::Pre::CalcTextSizeA(const ::ImFont *font, float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining, ImVec2 &retr) { ZoneScoped;
        if(!useVectorCmd) {
            return true;
        }
        IM_ASSERT(font != nullptr && "font is nullptr");

        if (!text_end) {
            text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.
        }

        if(remaining != nullptr) {
            *remaining = nullptr;
        }

        bool freeAllocatedText = false;

        if(isPasswordFont(*font)) {
            // assumes passwords are rendered on a single line
            freeAllocatedText = populatePasswordText(*font, &text_begin, &text_end, static_cast<size_t>(text_end-text_begin));
        } else if(wrap_width > 0.0f || isParagraphText(text_begin,text_end)) { ZoneScoped;
            if(wrap_width <= 0.0f) {
                wrap_width = ImGui::GetContentRegionAvail().x;
            }
            if(wrap_width <= 0.0) {
                retr.x = 0.0f;
                retr.y = size;
                return false;
            }
            paragraph->setFontSize(size);
            paragraph->build(text_begin,static_cast<size_t>(text_end-text_begin));
            paragraph->layout(SkFloatToScalar(wrap_width));
            retr.x = SkScalarToFloat(paragraph->getMaxIntrinsicWidth());
            retr.y = SkScalarToFloat(paragraph->getHeight());
            return false;
        }

        { ZoneScoped;
            const auto f = ImGui::skiaFont.makeWithSize(SkFloatToScalar(size));
            SkRect r;
            const SkScalar advanceWidth = f.measureText(text_begin,text_end-text_begin,SkTextEncoding::kUTF8, &r);
            if(freeAllocatedText) {
                IM_FREE(const_cast<char*>(text_begin));
            }

            if(!textMeasureModeXStack.empty()) {
                switch(textMeasureModeXStack.back()) {
                case ImZeroFB::TextMeasureModeX_AdvanceWidth:
                    retr.x = SkScalarToFloat(advanceWidth);
                    break;
                case ImZeroFB::TextMeasureModeX_BondingBox:
                    retr.x = r.width();
                    break;
                }
                switch(textMeasureModeYStack.back()) {
                case ImZeroFB::TextMeasureModeY_FontSize:
                    retr.y = size;
                    break;
                case ImZeroFB::TextMeasureModeY_BondingBox:
                    retr.y = r.height();
                    break;
                }
            } else {
                retr.x = SkScalarToFloat(advanceWidth);
                retr.y = size;
            }
        }
        return false;
    }

    bool Hooks::ImFont::Pre::RenderChar(const ::ImFont *font, ::ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, ImWchar c) { ZoneScoped;
        if(ImGui::useVectorCmd) {
            auto posFb = ImZeroFB::SingleVec2(pos.x,pos.y);
            auto clipRectFb = ImZeroFB::SingleVec4(0.0,0.0,0.0,0.0);
            auto arg = ImZeroFB::CreateCmdRenderUnicodeCodepoint(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(font),size,&posFb,col,&clipRectFb,static_cast<uint32_t>(c));
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRenderUnicodeCodepoint,arg.Union());
            return false;
        }
        return true;
    }

    bool Hooks::ImFont::Pre::RenderText(const ::ImFont *font, ::ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip) { ZoneScoped;
        if(!ImGui::useVectorCmd) {
            return true;
        }
        IM_ASSERT(font != nullptr && "font is nullptr");

        if (!text_end) {
            text_end = text_begin + strlen(text_begin);
        }

        auto const len = static_cast<size_t>(text_end-text_begin);
        if(pos.y > clip_rect.w) {
            return false;
        }

        auto posFb = ImZeroFB::SingleVec2(pos.x,pos.y);
        auto clipRectFb = ImZeroFB::SingleVec4(clip_rect.x,clip_rect.y,clip_rect.z,clip_rect.w);
        flatbuffers::Offset<flatbuffers::String> textFb;
        if(isPasswordFont(*font)) {
            initHiddenPwBuffer(*font);
            bool freeAllocatedText = populatePasswordText(*font,&text_begin,&text_end,text_end-text_begin);

            auto const arg = ImZeroFB::CreateCmdRenderText(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(font),size,&posFb,col,&clipRectFb,textFb);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRenderText,arg.Union());
            IM_FREE(const_cast<char*>(text_begin));
        } else {
            textFb = draw_list->fbBuilder->CreateString(text_begin,len);
            auto isParagraph = wrap_width > 0.0f || isParagraphText(text_begin,text_end);
            if(isParagraph && wrap_width <= 0.0f) {
                wrap_width = ImGui::GetContentRegionAvail().x;
                if(wrap_width <= 0.0f) {
                    // skip text, not visible
                    return false;
                }
            }
            if(isParagraph) {
//#define IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
#ifdef IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
                const bool renderAsParagraph = IMZERO_DRAWLIST_PARAGRAPH_AS_PATH;
#else
                constexpr bool renderAsParagraph = true;
#endif
                if(renderAsParagraph) {
                    ImZeroFB::TextAlignFlags align;
                    ImZeroFB::TextDirection dir;
                    getParagraphTextLayout(align,dir);
                    auto const arg = ImZeroFB::CreateCmdRenderParagraph(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(font),size,&posFb,col,&clipRectFb,textFb,wrap_width,0.0f,align, dir);
                    draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRenderParagraph,arg.Union());
                } else { ZoneScopedN("paragraphAsPath");
#ifdef IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
                    auto const clipRectSkia = SkRect::MakeLTRB(SkScalar(clip_rect.x),SkScalar(clip_rect.y),SkScalar(clip_rect.z),SkScalar(clip_rect.w));
            auto const clipRectSkiaTrans = clipRectSkia.makeOffset(-pos.x,-pos.y);

            ImGui::paragraph->setFontSize(SkScalar(size));
            ImGui::paragraph->build(text_begin,static_cast<size_t>(text_end-text_begin));
            ImGui::paragraph->layout(SkScalar(wrap_width));
            for(int lineNumber=0;;lineNumber++) {
                bool found;
                auto bounds = ImGui::paragraph->boundingRect(lineNumber,found);
                if(!found) {
                    break;
                }
                if(!bounds.intersect(clipRectSkiaTrans)) {
                    // clipped
                    continue;
                }

                //draw_list->AddRect(ImVec2(pos.x+ SkScalarToFloat(bounds.top()), pos.y+SkScalarToFloat(bounds.left())),
                //                         ImVec2(pos.x+SkScalarToFloat(bounds.right()), pos.y+SkScalarToFloat(bounds.bottom())),
                //                         0xaa1199ff,0.0f,0,2.0f);

                SkPath p;
                auto unrenderedGlyphs = ImGui::paragraph->getPath(lineNumber,p);
                /*
                // example data
                p.lineTo(1.0f,2.0f);
                p.conicTo(3.0f,4.0f,5.0f,6.0f,7.0f);
                p.cubicTo(8.0f,9.0f,10.0f,11.0f,12.0f,13.0f);
                p.quadTo(14.0f,15.0f,16.0f,18.0f);

                // output
                auto stream = SkFILEStream(stderr);
                p.dump((SkWStream*)&stream,true);
                p.dump(nullptr,true);

                // should produce the following output
                path.setFillType(SkPathFillType::kWinding);
                path.moveTo(SkBits2Float(0x00000000), SkBits2Float(0x00000000));  // 0, 0
                path.lineTo(SkBits2Float(0x3f800000), SkBits2Float(0x40000000));  // 1, 2
                path.conicTo(SkBits2Float(0x40400000), SkBits2Float(0x40800000), SkBits2Float(0x40a00000), SkBits2Float(0x40c00000), SkBits2Float(0x40e00000));  // 3, 4, 5, 6, 7
                path.cubicTo(SkBits2Float(0x41000000), SkBits2Float(0x41100000), SkBits2Float(0x41200000), SkBits2Float(0x41300000), SkBits2Float(0x41400000), SkBits2Float(0x41500000));  // 8, 9, 10, 11, 12, 13
                path.quadTo(SkBits2Float(0x41600000), SkBits2Float(0x41700000), SkBits2Float(0x41800000), SkBits2Float(0x41900000));  // 14, 15, 16, 18
                */
#if 0
                p.offset(SkScalar(pos.x),SkScalar(pos.y));
            auto svg = SkParsePath::ToSVGString(p);
            auto svgFb = draw_list->fbBuilder->CreateString(svg.data(),svg.size());
            auto arg = ImZeroFB::CreateCmdSvgPathSubset(*draw_list->fbBuilder,svgFb,col,true);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdSvgPathSubset,arg.Union());
#else
                static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_evenOdd) == static_cast<int64_t>(SkPathFillType::kEvenOdd));
                static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_winding) == static_cast<int64_t>(SkPathFillType::kWinding));
                static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_inverseEvenOdd) == static_cast<int64_t>(SkPathFillType::kInverseEvenOdd));
                static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_inverseWinding) == static_cast<int64_t>(SkPathFillType::kInverseWinding));
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_move) == static_cast<int64_t>( SkPath::Verb::kMove_Verb));
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_line) == static_cast<int64_t>( SkPath::Verb::kLine_Verb));
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_quad) == static_cast<int64_t>( SkPath::Verb::kQuad_Verb));
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_conic) == static_cast<int64_t>( SkPath::Verb::kConic_Verb));
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_cubic) == static_cast<int64_t>( SkPath::Verb::kCubic_Verb));
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_close) == static_cast<int64_t>( SkPath::Verb::kClose_Verb));
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_done) == static_cast<int64_t>( SkPath::Verb::kDone_Verb));
                static_assert(sizeof(ImZeroFB::PathVerb) == 1);

                auto const nVerbs = p.countVerbs();
                draw_list->fPathVerbBuffer.resize(0);
                draw_list->fPathVerbBuffer.reserve(nVerbs);
                draw_list->fPathWeightBuffer.resize(0);
                draw_list->fPathWeightBuffer.reserve(nVerbs); // upper bound, only needed for conic
                auto const nPoints = p.countPoints();
                draw_list->fPathPointBuffer.resize(0);
                draw_list->fPathPointBuffer.reserve(nPoints*2);
                static_assert(sizeof(SkPoint) == 2*sizeof(float));

                SkPath::Iter iter(p, false);
                SkPoint pts[4];
                SkPath::Verb verb;
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_move) == 0);
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_line) == 1);
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_quad) == 2);
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_conic) == 3);
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_cubic) == 4);
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_close) == 5);
                static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_done) == 6);
                constexpr int nPointsLU[SkPath::kDone_Verb+1] = {1, /* move */
                                                                 1, /* line */
                                                                 2, /* quad */
                                                                 2, /* conic */
                                                                 3, /* cubic */
                                                                 0, /* close */
                                                                 0 /* done */
                };

                // NOTE: iter seems to be the only method to get conic weights.
                // live would be much easier if methods p.getWeight(),p.getWeights() and p.getWeightCounts()
                // would exist --> skia pull request?
                while ((verb = iter.next(pts)) != SkPath::kDone_Verb) {
                    draw_list->fPathVerbBuffer.push_back(verb);
                    const int o = verb == SkPath::kMove_Verb ? 0 : 1;
                    // TODO optionally use SkPath::ConvertConicToQuads() to approximate conics (hyperbolic,elliptic,parabolic) using quadratic bezier curves
                    for(int i=0;i<nPointsLU[verb];i++) {
                        draw_list->fPathPointBuffer.push_back(pts[o+i].x());
                        draw_list->fPathPointBuffer.push_back(pts[o+i].y());
                    }
                    if(verb == SkPath::kConic_Verb) {
                        draw_list->fPathWeightBuffer.push_back(iter.conicWeight());
                    }
                    pts[0] = SkPoint::Make(-2.0f,-2.0f);
                    pts[1] = SkPoint::Make(-2.0f,-2.0f);
                    pts[2] = SkPoint::Make(-2.0f,-2.0f);
                    pts[3] = SkPoint::Make(-2.0f,-2.0f);
                }

                auto const pointXYs = draw_list->fbBuilder->CreateVector<float>(draw_list->fPathPointBuffer.Data,nPoints*2);
                auto const verbs = draw_list->fbBuilder->CreateVector<uint8_t>(draw_list->fPathVerbBuffer.Data,nVerbs);
                auto const weights = draw_list->fbBuilder->CreateVector<float>(draw_list->fPathWeightBuffer.Data,draw_list->fPathWeightBuffer.Size);

                auto arg = ImZeroFB::CreateCmdPath(*draw_list->fbBuilder,
                                                      &posFb,
                                                      verbs,
                                                      pointXYs,
                                                      weights,
                                                      col,
                                                      false,
                                                      true,
                                                      static_cast<ImZeroFB::PathFillType>(p.getFillType()));
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdPath,arg.Union());
#endif

            }
#endif
                }
            } else {
                auto const arg = ImZeroFB::CreateCmdRenderText(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(font),size,&posFb,col,&clipRectFb,textFb);
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRenderText,arg.Union());
            }
        }
        return false;
    }
}
