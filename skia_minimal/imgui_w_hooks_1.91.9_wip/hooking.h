#pragma once

#ifdef IMGUI_HOOK_ENABLE
#define IMGUI_HOOK_DRAW_LIST_PRE(r) if(!(r)) { return; }
#define IMGUI_HOOK_DRAW_LIST_POST(r) (r);
#define IMGUI_HOOK_DRAW_LIST_SPLITTER
#define IMGUI_HOOK_FONT_PRE(r) if(!(r)) { return retr; }
#define IMGUI_HOOK_GLOBAL_PRE(r) if(!(r)) { return; }
#define IMGUI_HOOK_GLOBAL_PRE_RETR(r) if(!(r)) { return retr; }
#else
#define IMGUI_HOOK_DRAW_LIST_PRE(r)
#define IMGUI_HOOK_DRAW_LIST_POST(r)
#define IMGUI_HOOK_FONT_PRE(r)
#define IMGUI_HOOK_GLOBAL_PRE(r)
#define IMGUI_HOOK_GLOBAL_PRE_RETR(r)
#endif

struct ImGuiWindow;

namespace ImGui {
    namespace Hooks {
        namespace Global {
            void ShouldAddDrawListToDrawData(const ::ImDrawList *draw_list, bool &shouldAdd);
            namespace Pre {
               bool RenderDimmedBackdgroundBehindWindow(::ImGuiWindow *window,ImU32 col);
               bool InputTextCalcTextSize(::ImVec2 &out, ::ImGuiContext* ctx, const char* text_begin, const char* text_end, const char** remaining=nullptr, ImVec2* out_offset=nullptr, bool stop_on_new_line=false);
            }
        }
        namespace ImDrawListSplitter {
            void SaveDrawListToSplitter(::ImDrawListSplitter &splitter, const ::ImDrawList *draw_list, int idx);
            void RestoreDrawListFromSplitter(::ImDrawListSplitter const &splitter, ::ImDrawList *draw_list, int idx);
            int EnsureSlotsCapacity(::ImDrawListSplitter &splitter, int nSlotsTotal);
            void ResetSlot(::ImDrawListSplitter &splitter,int i);
            void InitSlot(::ImDrawListSplitter &splitter,int i);
            void InitSlots(::ImDrawListSplitter &splitter, int nSlotsToInit);
            void ClearFreeMemory(::ImDrawListSplitter &splitter);
            int MergeInitialValue(::ImDrawListSplitter &splitter,::ImDrawList *drawList);
            int MergeUpdate(::ImDrawListSplitter &splitter,int i,int prev);
            void MergeReserve(::ImDrawList *drawList,int n);
            void MergeOp(::ImDrawListSplitter &splitter,::ImDrawList *drawList,int i);
        }
        namespace ImFont {
            namespace Pre {
                bool CalcTextSizeA(const ::ImFont *font, float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining, ImVec2 &retr);
                bool RenderChar(const ::ImFont *font, ::ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, ImWchar c);
                bool RenderText(const ::ImFont *font, ::ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip);
                bool GetCharAdvance(const ::ImFont *font, float &retr, ImWchar c);
            }
        }
        namespace ImDrawList {
            namespace Pre {

                bool AddDrawCmd(::ImDrawList *draw_list);
                bool _PopUnusedDrawCmd(::ImDrawList *draw_list);
                bool _ResetForNewFrame(::ImDrawList *draw_list);

                bool PushClipRect(::ImDrawList *draw_list,const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect = false);  // Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::PushClipRect(ImDrawList *draw_list,) to affect logic (ImDrawList *draw_list,hit-testing and widget culling)
                bool PushClipRectFullScreen(::ImDrawList *draw_list);
                bool PopClipRect(::ImDrawList *draw_list);
                bool PushTextureID(::ImDrawList *draw_list,ImTextureID texture_id);
                bool PopTextureID(::ImDrawList *draw_list);

                bool AddLine(::ImDrawList *draw_list,const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness);
                bool AddRect(::ImDrawList *draw_list,const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags, float thickness);
                bool AddRectFilled(::ImDrawList *draw_list,const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags);
                bool AddRectFilledMultiColor(::ImDrawList *draw_list,const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
                bool AddQuad(::ImDrawList *draw_list,const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness);
                bool AddQuadFilled(::ImDrawList *draw_list,const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col);
                bool AddTriangle(::ImDrawList *draw_list,const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness);
                bool AddTriangleFilled(::ImDrawList *draw_list,const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col);
                bool AddCircle(::ImDrawList *draw_list,const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness);
                bool AddCircleFilled(::ImDrawList *draw_list,const ImVec2& center, float radius, ImU32 col, int num_segments);
                bool AddNgon(::ImDrawList *draw_list,const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness);
                bool AddNgonFilled(::ImDrawList *draw_list,const ImVec2& center, float radius, ImU32 col, int num_segments);
                bool AddEllipse(::ImDrawList *draw_list,const ImVec2& center, const ImVec2& radius, ImU32 col, float rot, int num_segments, float thickness);
                bool AddEllipseFilled(::ImDrawList *draw_list,const ImVec2& center, const ImVec2& radius, ImU32 col, float rot, int num_segments);
                bool AddBezierCubic(::ImDrawList *draw_list,const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments);
                bool AddBezierQuadratic(::ImDrawList *draw_list,const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness, int num_segments);

                bool AddPolyline(::ImDrawList *draw_list,const ImVec2* points, int num_points, ImU32 col, ImDrawFlags flags, float thickness);
                bool AddConvexPolyFilled(::ImDrawList *draw_list,const ImVec2* points, int num_points, ImU32 col);
                bool AddConcavePolyFilled(::ImDrawList *draw_list,const ImVec2* points, int num_points, ImU32 col);

                bool AddImage(::ImDrawList *draw_list,ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col);
                bool AddImageQuad(::ImDrawList *draw_list,ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1, const ImVec2& uv2, const ImVec2& uv3, const ImVec2& uv4, ImU32 col);
                bool AddImageRounded(::ImDrawList *draw_list,ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawFlags flags);

                bool PathClear(::ImDrawList *draw_list);
                bool PathLineTo(::ImDrawList *draw_list,const ImVec2& pos);
                bool PathLineToMergeDuplicate(::ImDrawList *draw_list,const ImVec2& pos);
                bool PathFillConvex(::ImDrawList *draw_list,ImU32 col);
                bool PathFillConcave(::ImDrawList *draw_list,ImU32 col);
                bool PathStroke(::ImDrawList *draw_list,ImU32 col, ImDrawFlags flags, float thickness);
                bool PathArcTo(::ImDrawList *draw_list,const ImVec2& center, float radius, float a_min, float a_max, int num_segments);
                bool PathArcToFast(::ImDrawList *draw_list,const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12);
                bool PathEllipticalArcTo(::ImDrawList *draw_list,const ImVec2& center, const ImVec2& radius, float rot, float a_min, float a_max, int num_segments);
                bool PathBezierCubicCurveTo(::ImDrawList *draw_list,const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments);
                bool PathBezierQuadraticCurveTo(::ImDrawList *draw_list,const ImVec2& p2, const ImVec2& p3, int num_segments);
                bool PathRect(::ImDrawList *draw_list,const ImVec2& rect_min, const ImVec2& rect_max, float rounding, ImDrawFlags flags);

                bool AddCallback(::ImDrawList *draw_list,void *callback, void* userdata, size_t userdata_size);
                bool AddDrawCmd(::ImDrawList *draw_list);

                bool PrimReserve(::ImDrawList *draw_list,int idx_count, int vtx_count);
                bool PrimUnreserve(::ImDrawList *draw_list,int idx_count, int vtx_count);
                bool PrimRect(::ImDrawList *draw_list,const ImVec2& a, const ImVec2& b, ImU32 col);
                bool PrimRectUV(::ImDrawList *draw_list,const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col);
                bool PrimQuadUV(::ImDrawList *draw_list,const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col);
                bool PrimWriteVtx(::ImDrawList *draw_list,const ImVec2& pos, const ImVec2& uv, ImU32 col);
                bool PrimWriteIdx(::ImDrawList *draw_list,ImDrawIdx idx);
                bool PrimVtx(::ImDrawList *draw_list,const ImVec2& pos, const ImVec2& uv, ImU32 col);

                bool _ResetForNewFrame(::ImDrawList *draw_list);
                bool _ClearFreeMemory(::ImDrawList *draw_list);
                bool CloneOutput(::ImDrawList *draw_list,::ImDrawList *dest);
                bool _PopUnusedDrawCmd(::ImDrawList *draw_list);
                bool _TryMergeDrawCmds(::ImDrawList *draw_list);
                bool _OnChangedClipRect(::ImDrawList *draw_list);
                bool _OnChangedTextureID(::ImDrawList *draw_list);
                bool _OnChangedVtxOffset(::ImDrawList *draw_list);
            }

            namespace Post {
                void CreateImDrawList(::ImDrawList *draw_list, ImDrawListSharedData* shared_data);
                void CloneOutput(const ::ImDrawList *draw_list,::ImDrawList *dest);
                void PushClipRect(::ImDrawList *draw_list,const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect = false);
                void PopClipRect(::ImDrawList *draw_list);
            }
        }
    }
}
