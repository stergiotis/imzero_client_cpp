#pragma once
#include "imgui.h"
#include "imgui_skia_imzero_cmd_render.h"

typedef int SaveFormatE;
enum SaveFormatE_ {
    SaveFormatE_None = 0,
    SaveFormatE_SKP = 1,
    SaveFormatE_SVG = 2,
    SaveFormatE_SVG_TextAsPath = 3,
    SaveFormatE_PNG = 4,
    SaveFormatE_JPEG = 5,
    SaveFormatE_VECTORCMD = 6,
    SaveFormatE_Disabled = 7,
};
const char *GetSaveFormatName(SaveFormatE format);
const char *GetSaveFormatName(SaveFormatE format, size_t &len);
const char *GetSaveFormatExtension(SaveFormatE format);
const char *GetSaveFormatExtension(SaveFormatE format, size_t &len);

class SetupUI {
public:
    SetupUI();
    ~SetupUI();

    void render(SaveFormatE &saveFormat, VectorCmdSkiaRenderer &vectorCmdSkiaRenderer,bool &useVectorCmd,
                size_t totalVectorCmdSerializedSize, size_t fTotalFffiSz,
                size_t skpBytes, size_t svgBytes, size_t pngBytes, size_t jpegBytes,
                int windowW, int windowH, SkFontMgr *fontMgr=nullptr, const char *basePath = nullptr);

private:
    char fontMetricsText[128]{};
    float fontMetricsSize;
    ImVec4 colSize,colAscent,colDescent,colLeading,colXHeight,colCapHeight;
    ImZeroFB::TextAlignFlags fTextAlign = ImZeroFB::TextAlignFlags_Left;
    sk_sp<SkPicture> fSamplePicture;
    sk_sp<SkSurface> fSampleSurface;
};