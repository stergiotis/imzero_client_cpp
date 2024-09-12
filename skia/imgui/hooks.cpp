#include "tracy/Tracy.hpp"
#include "imgui_internal.h"
#include "imzero_draw_utils.h"
#include "../contrib/imgui/hooking.h"

#define IMZERO_DRAWLIST_BEGIN if(ImGui::useVectorCmd) {
#define IMZERO_DRAWLIST_END return false; }

namespace ImGui {
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
            auto arg = ImZeroFB::CreateCmdEllipseFilled(*draw_list->fbBuilder,&centerFb,&radius,col,rot,num_segments);
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
            auto arg = ImZeroFB::CreateCmdImage(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(user_texture_id),&pMinFb,&pMaxFb,&uvMinFb,&uvMaxFb,col);
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
            auto arg = ImZeroFB::CreateCmdImageQuad(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(user_texture_id),&p1Fb,&p2Fb,&p3Fb,&p4Fb,&uv1Fb,&uv2Fb,&uv3Fb,&uv4Fb,col);
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
            auto arg = ImZeroFB::CreateCmdImageRounded(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(user_texture_id),&pMinFb,&pMaxFb,&uvMinFb,&uvMaxFb,col,rounding,flags);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdImageRounded,arg.Union());
        IMZERO_DRAWLIST_END
        return true;
    }

    bool Hooks::ImFont::Pre::GetCharAdvance(const ::ImFont *font, float &retr, ImWchar c) { ZoneScoped;
        if(!ImGui::useVectorCmd) {
            return false;
        }

        uint32_t tmp;
        if(font->Glyphs.empty()) {
            // password font
            tmp = ImGui::skiaPasswordDefaultCharacter;
        } else {
            tmp = c;
        }
        auto const f = ImGui::skiaFont.makeWithSize(SkScalar(font->FontSize));
        auto const glyph = f.unicharToGlyph(SkUnichar(tmp));
        SkScalar advanceX;
        f.getWidths(&glyph,1,&advanceX);
        retr = SkScalarToFloat(advanceX);
        return true;
    }
}
