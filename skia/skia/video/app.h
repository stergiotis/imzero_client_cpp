#pragma once

#include "cliOptions.h"
#include "SkSurface.h"
#include "SkPaint.h"
#include "vectorCmdSkiaRenderer.h"
#include "setupUI.h"

enum rawFrameOutputFormat {
    kRawFrameOutputFormat_None = 0,
    kRawFrameOutputFormat_WebP_Lossless = 1,
    kRawFrameOutputFormat_WebP_Lossy = 2,
    kRawFrameOutputFormat_Jpeg = 3,
    kRawFrameOutputFormat_Png = 4,
    kRawFrameOutputFormat_Qoi = 5,
    kRawFrameOutputFormat_Bmp_Bgra8888 = 6,
    kRawFrameOutputFormat_PAM = 7,
    kRawFrameOutputFormat_Flatbuffers = 8,
};

class App {
public:
    App();
    ~App() = default;
    int Run(CliOptions &opts);
private:
    void Prepaint();
    void Paint(SkSurface* surface, int width, int height);
    void DrawImGuiVectorCmdsFB(SkCanvas &canvas);

    SkPaint fFontPaint;
    VectorCmdSkiaRenderer fVectorCmdSkiaRenderer;
    size_t fTotalVectorCmdSerializedSize;
    size_t fSkpBytesWritten;
    size_t fSvgBytesWritten;
    size_t fPngBytesWritten;
    ImZeroSkiaSetupUI fImZeroSkiaSetupUi;
    bool fFffiInterpreter;
    SkColor fBackgroundColor;
    bool fUseVectorCmd;
};