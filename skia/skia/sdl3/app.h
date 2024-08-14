#pragma once

#include "include/core/SkSurface.h"
#include "../cliOptions.h"
#include "../vectorCmdSkiaRenderer.h"
#include "../setupUI.h"

class App {
public:
    App();
    ~App() = default;
    int Run(CliOptions &opts);
private:
    void Paint(SkSurface* surface, int width, int height);
    void DrawImGuiVectorCmdsFB(SkCanvas &canvas);

    sk_sp<SkFontMgr> fFontMgr;
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