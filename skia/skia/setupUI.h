#pragma once
#include "imgui.h"
#include "vectorCmdSkiaRenderer.h"
#include "utils/SkEventTracer.h"

template <typename T, int n>
class ScrollingBuffer {
    int Offset;
    std::array<T,n> xs;
    std::array<T,n> ys;
    ScrollingBuffer() : Offset(0) {
    }
    void Add(T x, T y) {
        xs[Offset] = x;
        ys[Offset] = y;
    }
};
typedef int SaveFormatE;
enum SaveFormatE_ {
    SaveFormatE_None = 0,
    SaveFormatE_SKP = 1,
    SaveFormatE_SVG = 2,
    SaveFormatE_PNG = 3,
    SaveFormatE_VECTORCMD = 4,
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

    std::array<float,512> fBandwidthArena;
    std::array<float,512> fBandwidthVectorCmds;
    int fBandwidthPos = 0;
};