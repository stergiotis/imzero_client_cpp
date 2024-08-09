#include "imzero_draw_list.h"
#include <cstdint>

#include "tracy/Tracy.hpp"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "imgui_internal.h"
#include "imzero_draw_utils.h"

namespace ImGui {
    SkFont skiaFont;
    bool useVectorCmd = false;
    float skiaFontDyFudge = 0.0f;
    std::shared_ptr<Paragraph> paragraph = nullptr;
}

bool ImZeroDrawList::addVerticesAsVectorCmd() {
    auto sz = _delegate->CmdBuffer.Size;
    if(sz >= 2) {
        _TryMergeDrawCmds();
    }
    bool added = false;
    for(int i=0;i<sz;i++) {
        auto &cur = _delegate->CmdBuffer.Data[i];
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
void ImZeroDrawList::addVectorCmdFB(ImZeroFB::VectorCmdArg arg_type, flatbuffers::Offset<void> arg) {
    if(enableVectorCmdFBVertexDraw && ImGui::useVectorCmd) {
        if(addVerticesAsVectorCmd()) {
            _delegate->CmdBuffer.clear();
            AddDrawCmd();
        }
    }
    //fprintf(stderr,"%s: adding %s\n", _OwnerName, ImZeroFB::EnumNameVectorCmdArg(arg_type));
    _FbCmds->push_back(ImZeroFB::CreateSingleVectorCmdDto(*fbBuilder,arg_type,arg));
}
void ImZeroDrawList::serializeFB(const uint8_t *&out,size_t &size) { ZoneScoped;
    if(!ImGui::useVectorCmd) {
        // no native drawing commands, add all vertices as command (this will emulate the standard ImGui backend implementation)
        addVerticesAsVectorCmd();
    }

    auto dlFb = createVectorCmdFBDrawList(*this,false,*_FbCmds,*fbBuilder);
    fbBuilder->FinishSizePrefixed(dlFb,nullptr);
    size = fbBuilder->GetSize();
    out = fbBuilder->GetBufferPointer();
}

// Initialize before use in a new frame. We always have a command ready in the buffer.
void ImZeroDrawList::_ResetForNewFrame()
{ ZoneScoped;
    _FbProcessedDrawCmdIndexOffset = 0;
    if(fbBuilder == nullptr) {
        fbBuilder = new flatbuffers::FlatBufferBuilder();
    } else {
        fbBuilder->Clear();
    }
    if(_FbCmds == nullptr) {
        _FbCmds = new std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>>();
    } else {
        _FbCmds->resize(0);
    }
    _delegate->_ResetForNewFrame();
}

void ImZeroDrawList::_ClearFreeMemory()
{ ZoneScoped;
    if(fbBuilder != nullptr) {
        fbBuilder->Reset();
        delete fbBuilder;
        fbBuilder = nullptr;
    }
    if(_FbCmds != nullptr) {
        _FbCmds->clear();
        delete _FbCmds;
        _FbCmds = nullptr;
    }
#ifdef IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
    fPathVerbBuffer.clear();
    fPathPointBuffer.clear();
    fPathWeightBuffer.clear();
#endif
    _delegate->_ClearFreeMemory();
}

ImDrawList* ImZeroDrawList::CloneOutput() const
{ ZoneScoped;
    // FIXME
    return _delegate->CloneOutput();
}

void ImZeroDrawList::AddDrawCmd()
{ ZoneScoped;
    _delegate->AddDrawCmd();
}

void ImZeroDrawList::_PopUnusedDrawCmd()
{ ZoneScoped;
    _delegate->_PopUnusedDrawCmd();
}

void ImZeroDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{ ZoneScoped;
    _delegate->AddCallback(callback,callback_data);
}

// Try to merge two last draw commands
void ImZeroDrawList::_TryMergeDrawCmds()
{ ZoneScoped;
    _delegate->_TryMergeDrawCmds();
}

void ImZeroDrawList::_OnChangedClipRect()
{ ZoneScoped;
    _delegate->_OnChangedClipRect();
}

void ImZeroDrawList::_OnChangedTextureID()
{ ZoneScoped;
    _delegate->_OnChangedTextureID();
}

void ImZeroDrawList::_OnChangedVtxOffset()
{ ZoneScoped;
    _delegate->_OnChangedVtxOffset();
}


void ImZeroDrawList::PushClipRect(const ImVec2& cr_min, const ImVec2& cr_max, bool intersect_with_current_clip_rect)
{ ZoneScoped;
    _delegate->PushClipRect(cr_min,cr_max,intersect_with_current_clip_rect);

    auto const cr = _delegate->_CmdHeader.ClipRect;

    auto rect = ImZeroFB::SingleVec4(cr.x,cr.y,cr.z,cr.w); // NOTE: use of intersected rectangle is mandatory
    auto arg = ImZeroFB::CreateCmdPushClipRect(*fbBuilder,&rect,intersect_with_current_clip_rect);
    addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdPushClipRect,arg.Union());
}

void ImZeroDrawList::PushClipRectFullScreen()
{ ZoneScoped;
    _delegate->PushClipRectFullScreen();
}

void ImZeroDrawList::PopClipRect()
{ ZoneScoped;
    _delegate->PopClipRect();

    if(fbBuilder != nullptr) {
        auto arg = ImZeroFB::CreateCmdPopClipRect(*fbBuilder);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdPopClipRect,arg.Union());
    }
}

void ImZeroDrawList::PushTextureID(ImTextureID texture_id)
{ ZoneScoped;
    _delegate->PushTextureID(texture_id);
}

void ImZeroDrawList::PopTextureID()
{ ZoneScoped;
    _delegate->PopTextureID();
}

void ImZeroDrawList::PrimReserve(int idx_count, int vtx_count)
{ ZoneScoped;
    _delegate->PrimReserve(idx_count,vtx_count);
}

void ImZeroDrawList::PrimUnreserve(int idx_count, int vtx_count)
{ ZoneScoped;
    _delegate->PrimUnreserve(idx_count,vtx_count);
}

// Fully unrolled with inline call to keep our debug builds decently fast.
void ImZeroDrawList::PrimRect(const ImVec2& a, const ImVec2& c, ImU32 col)
{ ZoneScoped;
    _delegate->PrimRect(a,c,col);
}

void ImZeroDrawList::PrimRectUV(const ImVec2& a, const ImVec2& c, const ImVec2& uv_a, const ImVec2& uv_c, ImU32 col)
{ ZoneScoped;
    _delegate->PrimRectUV(a,c,uv_a,uv_c,col);
}

void ImZeroDrawList::PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col)
{ ZoneScoped;
    _delegate->PrimQuadUV(a,b,c,d,uv_a,uv_b,uv_c,uv_d,col);
}

void ImZeroDrawList::AddPolyline(const ImVec2* points, const int points_count, ImU32 col, ImDrawFlags flags, float thickness)
{ ZoneScoped;
    if (points_count < 2 || (col & IM_COL32_A_MASK) == 0) {
        return;
    }

    if(ImGui::useVectorCmd) {
        flatbuffers::Offset<flatbuffers::Vector<float>> xs, ys;
        fbAddPointsToVector(xs,ys,*fbBuilder,points,points_count);
        auto p = ImZeroFB::CreateArrayOfVec2(*fbBuilder,xs,ys);
        auto arg = ImZeroFB::CreateCmdPolyline(*fbBuilder,p,col,flags,thickness);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdPolyline,arg.Union());
    } else {
        _delegate->AddPolyline(points,points_count,col,flags,thickness);
    }
}

// - We intentionally avoid using ImVec2 and its math operators here to reduce cost to a minimum for debug/non-inlined builds.
// - Filled shapes must always use clockwise winding order. The anti-aliasing fringe depends on it. Counter-clockwise shapes will have "inward" anti-aliasing.
void ImZeroDrawList::AddConvexPolyFilled(const ImVec2* points, const int points_count, ImU32 col)
{ ZoneScoped;
    if (points_count < 3 || (col & IM_COL32_A_MASK) == 0) {
        return;
    }
    if(ImGui::useVectorCmd) {
        flatbuffers::Offset<flatbuffers::Vector<float>> xs, ys;
        fbAddPointsToVector(xs,ys,*fbBuilder,points,points_count);
        auto p = ImZeroFB::CreateArrayOfVec2(*fbBuilder,xs,ys);
        auto arg = ImZeroFB::CreateCmdConvexPolyFilled(*fbBuilder,p,col);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdConvexPolyFilled,arg.Union());
    } else {
        _delegate->AddConvexPolyFilled(points,points_count,col);
    }
}

void ImZeroDrawList::_PathArcToFastEx(const ImVec2& center, float radius, int a_min_sample, int a_max_sample, int a_step)
{ ZoneScoped;
    _delegate->_PathArcToFastEx(center,radius,a_min_sample,a_max_sample,a_step);
}

void ImZeroDrawList::_PathArcToN(const ImVec2& center, float radius, float a_min, float a_max, int num_segments)
{ ZoneScoped;
    _delegate->_PathArcToN(center,radius,a_min,a_max,num_segments);
}

void ImZeroDrawList::PathArcToFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12)
{ ZoneScoped;
    // FIXME
    _delegate->PathArcToFast(center,radius,a_min_of_12,a_max_of_12);
}

void ImZeroDrawList::PathArcTo(const ImVec2& center, float radius, float a_min, float a_max, int num_segments)
{ ZoneScoped;
    // FIXME
    _delegate->PathArcTo(center,radius,a_min,a_max,num_segments);
}

void ImZeroDrawList::PathEllipticalArcTo(const ImVec2& center, float radius_x, float radius_y, float rot, float a_min, float a_max, int num_segments)
{ ZoneScoped;
    // FIXME
    _delegate->PathEllipticalArcTo(center,radius_x,radius_y,rot,a_min,a_max,num_segments);
}

void ImZeroDrawList::PathBezierCubicCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments)
{ ZoneScoped;
    // FIXME
    _delegate->PathBezierCubicCurveTo(p2,p3,p4,num_segments);
}

void ImZeroDrawList::PathBezierQuadraticCurveTo(const ImVec2& p2, const ImVec2& p3, int num_segments)
{ ZoneScoped;
    // FIXME
    _delegate->PathBezierQuadraticCurveTo(p2,p3,num_segments);
}

void ImZeroDrawList::PathRect(const ImVec2& a, const ImVec2& b, float rounding, ImDrawFlags flags)
{ ZoneScoped;
    // FIXME
    _delegate->PathRect(a,b,rounding,flags);
}

void ImZeroDrawList::AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
        auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
        auto arg = ImZeroFB::CreateCmdLine(*fbBuilder,&p1Fb,&p2Fb,col,thickness);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdLine,arg.Union());
        return;
    } else {
        _delegate->AddLine(p1,p2,col,thickness);
    }
}

void ImZeroDrawList::AddRect(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags, float thickness)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
        auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
        if (rounding >= 0.5f) {
            //flags = FixRectCornerFlags(flags);
            rounding = std::min(rounding, fabs(p_max.x - p_min.x) * (((flags & ImDrawFlags_RoundCornersTop) == ImDrawFlags_RoundCornersTop) || ((flags & ImDrawFlags_RoundCornersBottom) == ImDrawFlags_RoundCornersBottom) ? 0.5f : 1.0f) - 1.0f);
            rounding = std::min(rounding, fabs(p_max.y - p_min.y) * (((flags & ImDrawFlags_RoundCornersLeft) == ImDrawFlags_RoundCornersLeft) || ((flags & ImDrawFlags_RoundCornersRight) == ImDrawFlags_RoundCornersRight) ? 0.5f : 1.0f) - 1.0f);
        }
        if (rounding < 0.5f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersNone) {
            rounding = 0.0f;
        }

        if(rounding == 0.0f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersAll) {
            auto arg = ImZeroFB::CreateCmdRectRounded(*fbBuilder,&pMinFb,&pMaxFb,col,rounding,thickness);
            addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectRounded,arg.Union());
        } else {
            const float rounding_tl = (flags & ImDrawFlags_RoundCornersTopLeft)     ? rounding : 0.0f;
            const float rounding_tr = (flags & ImDrawFlags_RoundCornersTopRight)    ? rounding : 0.0f;
            const float rounding_br = (flags & ImDrawFlags_RoundCornersBottomRight) ? rounding : 0.0f;
            const float rounding_bl = (flags & ImDrawFlags_RoundCornersBottomLeft)  ? rounding : 0.0f;
            auto arg = ImZeroFB::CreateCmdRectRoundedCorners(*fbBuilder,&pMinFb,&pMaxFb,col,rounding_tl,rounding_tr,rounding_br,rounding_bl,thickness);
            addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectRoundedCorners,arg.Union());
        }

        return;
    } else {
        _delegate->AddRect(p_min,p_max,col,rounding,flags,thickness);
    }
}

void ImZeroDrawList::AddRectFilled(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
       return;
    }

    if(ImGui::useVectorCmd) {
        auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
        auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
        if (rounding >= 0.5f) {
            //flags = FixRectCornerFlags(flags);
            rounding = std::min(rounding, fabs(p_max.x - p_min.x) * (((flags & ImDrawFlags_RoundCornersTop) == ImDrawFlags_RoundCornersTop) || ((flags & ImDrawFlags_RoundCornersBottom) == ImDrawFlags_RoundCornersBottom) ? 0.5f : 1.0f) - 1.0f);
            rounding = std::min(rounding, fabs(p_max.y - p_min.y) * (((flags & ImDrawFlags_RoundCornersLeft) == ImDrawFlags_RoundCornersLeft) || ((flags & ImDrawFlags_RoundCornersRight) == ImDrawFlags_RoundCornersRight) ? 0.5f : 1.0f) - 1.0f);
        }

        if (rounding < 0.5f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersNone) {
            rounding = 0.0f;
        }

        if(rounding == 0.0f || (flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersAll) {
            auto arg = ImZeroFB::CreateCmdRectRoundedFilled(*fbBuilder,&pMinFb,&pMaxFb,col,rounding);
            addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectRoundedFilled,arg.Union());
        } else {
            const float rounding_tl = (flags & ImDrawFlags_RoundCornersTopLeft)     ? rounding : 0.0f;
            const float rounding_tr = (flags & ImDrawFlags_RoundCornersTopRight)    ? rounding : 0.0f;
            const float rounding_br = (flags & ImDrawFlags_RoundCornersBottomRight) ? rounding : 0.0f;
            const float rounding_bl = (flags & ImDrawFlags_RoundCornersBottomLeft)  ? rounding : 0.0f;
            auto arg = ImZeroFB::CreateCmdRectRoundedCornersFilled(*fbBuilder,&pMinFb,&pMaxFb,col,rounding_tl,rounding_tr,rounding_br,rounding_bl);
            addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectRoundedCornersFilled,arg.Union());
        }

        return;
    } else {
        _delegate->AddRectFilled(p_min,p_max,col,rounding,flags);
    }
}

void ImZeroDrawList::AddRectFilledMultiColor(const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{ ZoneScoped;
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0) {
        return;
    }

    if(ImGui::useVectorCmd && false) { // FIXME
        auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
        auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
        auto arg = ImZeroFB::CreateCmdRectFilledMultiColor(*fbBuilder,&pMinFb,&pMaxFb,col_upr_left,col_upr_right,col_bot_right,col_bot_left);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRectFilledMultiColor,arg.Union());
    } else {
        _delegate->AddRectFilledMultiColor(p_min,p_max,col_upr_left,col_upr_right,col_bot_right,col_bot_left);
    }
}

void ImZeroDrawList::AddQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
        auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
        auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
        auto p4Fb = ImZeroFB::SingleVec2(p4.x,p4.y);
        auto arg = ImZeroFB::CreateCmdQuad(*fbBuilder,&p1Fb,&p2Fb,&p3Fb,&p4Fb,col,thickness);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdQuad,arg.Union());
    } else {
        _delegate->AddQuad(p1,p2,p3,p4,col,thickness);
    }
}

void ImZeroDrawList::AddQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
        auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
        auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
        auto p4Fb = ImZeroFB::SingleVec2(p4.x,p4.y);
        auto arg = ImZeroFB::CreateCmdQuadFilled(*fbBuilder,&p1Fb,&p2Fb,&p3Fb,&p4Fb,col);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdQuadFilled,arg.Union());
    } else {
        _delegate->AddQuadFilled(p1,p2,p3,p4,col);
    }
}

void ImZeroDrawList::AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
        auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
        auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
        auto arg = ImZeroFB::CreateCmdTriangle(*fbBuilder,&p1Fb,&p2Fb,&p3Fb,col);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdTriangle,arg.Union());
    } else {
        _delegate->AddTriangle(p1,p2,p3,col,thickness);
    }
}

void ImZeroDrawList::AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
       return;
    }

    if(ImGui::useVectorCmd) {
        auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
        auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
        auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
        auto arg = ImZeroFB::CreateCmdTriangleFilled(*fbBuilder,&p1Fb,&p2Fb,&p3Fb,col);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdTriangleFilled,arg.Union());
    } else {
        _delegate->AddTriangleFilled(p1,p2,p3,col);
    }
}

void ImZeroDrawList::AddCircle(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0 || radius < 0.5f) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
        auto arg = ImZeroFB::CreateCmdCircle(*fbBuilder,&centerFb,radius,col,num_segments,thickness);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdCircle,arg.Union());
    } else {
        _delegate->AddCircle(center,radius,col,num_segments,thickness);
    }
}

void ImZeroDrawList::AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0 || radius < 0.5f) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
        auto arg = ImZeroFB::CreateCmdCircleFilled(*fbBuilder,&centerFb,radius,col);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdCircleFilled,arg.Union());
    } else {
        _delegate->AddCircleFilled(center,radius,col,num_segments);
    }
}

void ImZeroDrawList::AddNgon(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
        auto arg = ImZeroFB::CreateCmdNgon(*fbBuilder,&centerFb,radius,col,num_segments,thickness);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdNgon,arg.Union());
    } else {
        _delegate->AddNgon(center,radius,col,num_segments,thickness);
    }
}

void ImZeroDrawList::AddNgonFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2) {
       return;
    }

    if(ImGui::useVectorCmd) {
        auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
        auto arg = ImZeroFB::CreateCmdNgonFilled(*fbBuilder,&centerFb,radius,col,num_segments);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdNgonFilled,arg.Union());
    } else {
        _delegate->AddNgonFilled(center,radius,col,num_segments);
    }
}

void ImZeroDrawList::AddEllipse(const ImVec2& center, float radius_x, float radius_y, ImU32 col, float rot, int num_segments, float thickness)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
       return;
    }
    if(ImGui::useVectorCmd) {
        auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
        auto arg = ImZeroFB::CreateCmdEllipse(*fbBuilder,&centerFb,radius_x,radius_y,col,rot,num_segments,thickness);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdEllipse,arg.Union());
    } else {
        _delegate->AddEllipse(center,radius_x,radius_y,col,rot,num_segments,thickness);
    }
}

void ImZeroDrawList::AddEllipseFilled(const ImVec2& center, float radius_x, float radius_y, ImU32 col, float rot, int num_segments)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto centerFb = ImZeroFB::SingleVec2(center.x,center.y);
        auto arg = ImZeroFB::CreateCmdEllipseFilled(*fbBuilder,&centerFb,radius_x,radius_y,col,rot,num_segments);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdEllipseFilled,arg.Union());
    } else {
        _delegate->AddEllipseFilled(center,radius_x,radius_y,col,rot,num_segments);
    }
}

void ImZeroDrawList::AddBezierCubic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
        return;
    }
    if(ImGui::useVectorCmd) {
        auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
        auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
        auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
        auto p4Fb = ImZeroFB::SingleVec2(p4.x,p4.y);
        auto arg = ImZeroFB::CreateCmdBezierCubic(*fbBuilder,&p1Fb,&p2Fb,&p3Fb,&p4Fb,col,thickness,num_segments);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdBezierCubic,arg.Union());
    } else {
        _delegate->AddBezierCubic(p1, p2, p3, p4, col, thickness, num_segments);
    }
}

void ImZeroDrawList::AddBezierQuadratic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness, int num_segments)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
       return;
    }
    if(ImGui::useVectorCmd) {
        auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
        auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
        auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
        auto arg = ImZeroFB::CreateCmdBezierQuadratic(*fbBuilder,&p1Fb,&p2Fb,&p3Fb,col,thickness,num_segments);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdBezierQuadratic,arg.Union());
    } else {
        _delegate->AddBezierQuadratic(p1,p2,p3,col,thickness,num_segments);
    }
}

void ImZeroDrawList::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{ ZoneScoped;
    _delegate->AddText(font,font_size,pos,col,text_begin,text_end,wrap_width,cpu_fine_clip_rect);
}

void ImZeroDrawList::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{ ZoneScoped;
    _delegate->AddText(nullptr, 0.0f, pos, col, text_begin, text_end);
}

void ImZeroDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
        return;
    }
    if(ImGui::useVectorCmd) { // FIXME
        auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
        auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
        auto uvMinFb = ImZeroFB::SingleVec2(uv_min.x,uv_min.y);
        auto uvMaxFb = ImZeroFB::SingleVec2(uv_max.x,uv_max.y);
        auto arg = ImZeroFB::CreateCmdImage(*fbBuilder,reinterpret_cast<uint64_t>(user_texture_id),&pMinFb,&pMaxFb,&uvMinFb,&uvMaxFb,col);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdImage,arg.Union());
    } else {
        _delegate->AddImage(user_texture_id,p_min,p_max,uv_min,uv_max,col);;
    }
}

void ImZeroDrawList::AddImageQuad(ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1, const ImVec2& uv2, const ImVec2& uv3, const ImVec2& uv4, ImU32 col)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
       return;
    }

    if(ImGui::useVectorCmd) {
        auto p1Fb = ImZeroFB::SingleVec2(p1.x,p1.y);
        auto p2Fb = ImZeroFB::SingleVec2(p2.x,p2.y);
        auto p3Fb = ImZeroFB::SingleVec2(p3.x,p3.y);
        auto p4Fb = ImZeroFB::SingleVec2(p4.x,p4.y);
        auto uv1Fb = ImZeroFB::SingleVec2(uv1.x,uv1.y);
        auto uv2Fb = ImZeroFB::SingleVec2(uv2.x,uv2.y);
        auto uv3Fb = ImZeroFB::SingleVec2(uv3.x,uv3.y);
        auto uv4Fb = ImZeroFB::SingleVec2(uv4.x,uv4.y);
        auto arg = ImZeroFB::CreateCmdImageQuad(*fbBuilder,reinterpret_cast<uint64_t>(user_texture_id),&p1Fb,&p2Fb,&p3Fb,&p4Fb,&uv1Fb,&uv2Fb,&uv3Fb,&uv4Fb,col);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdImageQuad,arg.Union());
    } else {
        _delegate->AddImageQuad(user_texture_id,p1,p2,p3,p4,uv1,uv2,uv3,uv4,col);
    }
}

void ImZeroDrawList::AddImageRounded(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawFlags flags)
{ ZoneScoped;
    if ((col & IM_COL32_A_MASK) == 0) {
        return;
    }

    if(ImGui::useVectorCmd) {
        auto pMinFb = ImZeroFB::SingleVec2(p_min.x,p_min.y);
        auto pMaxFb = ImZeroFB::SingleVec2(p_max.x,p_max.y);
        auto uvMinFb = ImZeroFB::SingleVec2(uv_min.x,uv_max.y);
        auto uvMaxFb = ImZeroFB::SingleVec2(uv_min.x,uv_max.y);
        auto arg = ImZeroFB::CreateCmdImageRounded(*fbBuilder,reinterpret_cast<uint64_t>(user_texture_id),&pMinFb,&pMaxFb,&uvMinFb,&uvMaxFb,col,rounding,flags);
        addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdImageRounded,arg.Union());
    } else {
        _delegate->AddImageRounded(user_texture_id,p_min,p_max,uv_min,uv_max,col,rounding,flags);
    }
}