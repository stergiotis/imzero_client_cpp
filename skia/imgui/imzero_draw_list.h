#pragma once

#include "imgui.h"
#include "include/core/SkSurface.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkFont.h"
#include "ImZeroFB.out.h"
#include "../skia/paragraph.h"
#include "imzero_config.h"

#define IMZERO_DRAWLIST

struct ImZeroDrawList {
    ImDrawList *_delegate = nullptr;

    ImZeroDrawList(ImDrawListSharedData* shared_data) {
        _delegate = IM_NEW(ImDrawList(shared_data));
    }
    ~ImZeroDrawList() {
        IM_DELETE(_delegate);
        _delegate = nullptr;
    }
    IMGUI_API void  PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect = false);
    IMGUI_API void  PushClipRectFullScreen();
    IMGUI_API void  PopClipRect();
    IMGUI_API void  PushTextureID(ImTextureID texture_id);
    IMGUI_API void  PopTextureID();
    inline ImVec2   GetClipRectMin() const { return _delegate->GetClipRectMin(); }
    inline ImVec2   GetClipRectMax() const { return _delegate->GetClipRectMax(); }

    IMGUI_API void  AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 1.0f);
    IMGUI_API void  AddRect(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f);
    IMGUI_API void  AddRectFilled(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0);
    IMGUI_API void  AddRectFilledMultiColor(const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
    IMGUI_API void  AddQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness = 1.0f);
    IMGUI_API void  AddQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col);
    IMGUI_API void  AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness = 1.0f);
    IMGUI_API void  AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col);
    IMGUI_API void  AddCircle(const ImVec2& center, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);
    IMGUI_API void  AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments = 0);
    IMGUI_API void  AddNgon(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness = 1.0f);
    IMGUI_API void  AddNgonFilled(const ImVec2& center, float radius, ImU32 col, int num_segments);
    IMGUI_API void  AddEllipse(const ImVec2& center, float radius_x, float radius_y, ImU32 col, float rot = 0.0f, int num_segments = 0, float thickness = 1.0f);
    IMGUI_API void  AddEllipseFilled(const ImVec2& center, float radius_x, float radius_y, ImU32 col, float rot = 0.0f, int num_segments = 0);
    IMGUI_API void  AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
    IMGUI_API void  AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = NULL);
    IMGUI_API void  AddPolyline(const ImVec2* points, int num_points, ImU32 col, ImDrawFlags flags, float thickness);
    IMGUI_API void  AddConvexPolyFilled(const ImVec2* points, int num_points, ImU32 col);
    IMGUI_API void  AddBezierCubic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments = 0);
    IMGUI_API void  AddBezierQuadratic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness, int num_segments = 0);

    IMGUI_API void  AddImage(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min = ImVec2(0, 0), const ImVec2& uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE);
    IMGUI_API void  AddImageQuad(ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1 = ImVec2(0, 0), const ImVec2& uv2 = ImVec2(1, 0), const ImVec2& uv3 = ImVec2(1, 1), const ImVec2& uv4 = ImVec2(0, 1), ImU32 col = IM_COL32_WHITE);
    IMGUI_API void  AddImageRounded(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawFlags flags = 0);

    inline    void  PathClear()                                                 { _delegate->PathClear(); }
    inline    void  PathLineTo(const ImVec2& pos)                               { _delegate->PathLineTo(pos); }
    inline    void  PathLineToMergeDuplicate(const ImVec2& pos)                 { _delegate->PathLineToMergeDuplicate(pos); }
    inline    void  PathFillConvex(ImU32 col)                                   { _delegate->PathFillConvex(col); }
    inline    void  PathStroke(ImU32 col, ImDrawFlags flags = 0, float thickness = 1.0f) { _delegate->PathStroke(col,flags,thickness); }
    IMGUI_API void  PathArcTo(const ImVec2& center, float radius, float a_min, float a_max, int num_segments = 0);
    IMGUI_API void  PathArcToFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12);
    IMGUI_API void  PathEllipticalArcTo(const ImVec2& center, float radius_x, float radius_y, float rot, float a_min, float a_max, int num_segments = 0);
    IMGUI_API void  PathBezierCubicCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments = 0);
    IMGUI_API void  PathBezierQuadraticCurveTo(const ImVec2& p2, const ImVec2& p3, int num_segments = 0);
    IMGUI_API void  PathRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding = 0.0f, ImDrawFlags flags = 0);

    IMGUI_API void  AddCallback(ImDrawCallback callback, void* callback_data);
    IMGUI_API void  AddDrawCmd();
    IMGUI_API ImDrawList* CloneOutput() const;

    inline void     ChannelsSplit(int count)    { _delegate->ChannelsSplit(count); }
    inline void     ChannelsMerge()             { _delegate->ChannelsMerge(); }
    inline void     ChannelsSetCurrent(int n)   { _delegate->ChannelsSetCurrent(n); }

    IMGUI_API void  PrimReserve(int idx_count, int vtx_count);
    IMGUI_API void  PrimUnreserve(int idx_count, int vtx_count);
    IMGUI_API void  PrimRect(const ImVec2& a, const ImVec2& b, ImU32 col);      // Axis aligned rectangle (composed of two triangles)
    IMGUI_API void  PrimRectUV(const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col);
    IMGUI_API void  PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col);
    inline    void  PrimWriteVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col)    { _delegate->PrimWriteVtx(pos,uv,col); }
    inline    void  PrimWriteIdx(ImDrawIdx idx)                                     { _delegate->PrimWriteIdx(idx); }
    inline    void  PrimVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col)         { _delegate->PrimVtx(pos,uv,col); }

    // [Internal helpers]
    IMGUI_API void  _ResetForNewFrame();
    IMGUI_API void  _ClearFreeMemory();
    IMGUI_API void  _PopUnusedDrawCmd();
    IMGUI_API void  _TryMergeDrawCmds();
    IMGUI_API void  _OnChangedClipRect();
    IMGUI_API void  _OnChangedTextureID();
    IMGUI_API void  _OnChangedVtxOffset();
    IMGUI_API int   _CalcCircleAutoSegmentCount(float radius) const;
    IMGUI_API void  _PathArcToFastEx(const ImVec2& center, float radius, int a_min_sample, int a_max_sample, int a_step);
    IMGUI_API void  _PathArcToN(const ImVec2& center, float radius, float a_min, float a_max, int num_segments);

    flatbuffers::FlatBufferBuilder *fbBuilder;
    std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>> *_FbCmds;
    uint32_t _FbProcessedDrawCmdIndexOffset;
public: // public: ImFont needs to access this method, in spirit of the rest of imgui I do not use C++'s friend keyword
    void addVectorCmdFB(ImZeroFB::VectorCmdArg arg_type, flatbuffers::Offset<void> arg);
    bool addVerticesAsVectorCmd();
    #ifdef IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
    ImVector<uint8_t> fPathVerbBuffer;
    ImVector<float> fPathPointBuffer;
    ImVector<float> fPathWeightBuffer;
    #endif
    // serializeFB: out is valid until next frame
    void serializeFB(const uint8_t *&out,size_t &sz);
};