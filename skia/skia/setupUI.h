#pragma once
#include "imgui.h"
#include "vectorCmdSkiaRenderer.h"
#include "utils/SkEventTracer.h"

typedef int SaveFormatE;
enum SaveFormatE_ {
    SaveFormatE_None = 0,
    SaveFormatE_SKP = 1,
    SaveFormatE_SVG = 2,
    SaveFormatE_SVGNoFont = 3,
    SaveFormatE_PNG = 4,
    SaveFormatE_VECTORCMD = 5,
};

class ImZeroSkiaSetupUI {
public:
    ImZeroSkiaSetupUI();
    ~ImZeroSkiaSetupUI();

    void render(SaveFormatE &saveFormat, VectorCmdSkiaRenderer &vectorCmdSkiaRenderer,bool &skiaBackendActive,
                size_t totalVectorCmdSerializedSize, size_t fTotalFffiSz,
                size_t skpBytes, size_t svgBytes, size_t pngBytes,
                int windowW, int windowH);

private:
    char fontMetricsText[128]{};
    float fontMetricsSize;
    ImVec4 colSize,colAscent,colDescent,colLeading,colXHeight,colCapHeight;
    SkEventTracer *fEventTracer = nullptr;
};