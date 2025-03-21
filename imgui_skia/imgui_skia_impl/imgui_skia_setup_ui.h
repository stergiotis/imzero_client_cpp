#pragma once
#include "imgui.h"
#include "imgui_skia_imzero_cmd_render.h"

namespace ImGuiSkia
{
    typedef int FrameExportFormatE;
    enum FrameExportFormatE_ {
        FrameExportFormatE_NoExport = 0,
        FrameExportFormatE_SKP = 1,
        FrameExportFormatE_SVG = 2,
        FrameExportFormatE_SVG_TextAsPath = 3,
        FrameExportFormatE_PNG = 4,
        FrameExportFormatE_JPEG = 5,
        FrameExportFormatE_VECTORCMD = 6,
        FrameExportFormatE_Disabled = 7,
    };
    const char *GetFrameExportFormatName(FrameExportFormatE format);
    const char *GetFrameExportFormatName(FrameExportFormatE format, size_t &len);
    const char *GetFrameExportFormatExtension(FrameExportFormatE format);
    const char *GetFrameExportFormatExtension(FrameExportFormatE format, size_t &len);

    class SetupUI {
    public:
        SetupUI();
        ~SetupUI();

        void render(FrameExportFormatE &exportFormat, VectorCmdSkiaRenderer &vectorCmdSkiaRenderer,bool &useVectorCmd,
                    size_t totalVectorCmdSerializedSize, size_t fTotalFffiSz,
                    size_t skpBytes, size_t fbBytes, size_t svgBytes, size_t pngBytes, size_t jpegBytes,
                    int windowW, int windowH, SkFontMgr *fontMgr=nullptr, const char *basePath = nullptr);

    private:
        char fFontMetricsText[128]{};
        float fFontMetricsSize = 0.0f;
        ImVec4 fColSize,fColAscent,fColDescent,fColLeading,fColXHeight,fColCapHeight;
        ImZeroFB::TextAlignFlags fTextAlign = ImZeroFB::TextAlignFlags_Left;
        sk_sp<SkPicture> fSamplePicture;
        sk_sp<SkSurface> fSampleSurface;
    };
}