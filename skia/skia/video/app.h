#pragma once

#include "cliOptions.h"
#include "SkSurface.h"
#include "SkPaint.h"
#include "vectorCmdSkiaRenderer.h"
#include "setupUI.h"
#include "userInteraction_generated.h"

enum rawFrameOutputFormat {
    kRawFrameOutputFormat_None = 0,
    kRawFrameOutputFormat_WebP_Lossless = 1,
    kRawFrameOutputFormat_WebP_Lossy = 2,
    kRawFrameOutputFormat_Jpeg = 3,
    kRawFrameOutputFormat_Png = 4,
    kRawFrameOutputFormat_Qoi = 5,
    kRawFrameOutputFormat_Bmp_Bgra8888 = 6,
    kRawFrameOutputFormat_Flatbuffers = 7,
    kRawFrameOutputFormat_Svg = 8,
    kRawFrameOutputFormat_Svg_TextAsPath = 9,
    kRawFrameOutputFormat_Skp = 10,
};

class App {
public:
    App();
    ~App();
    int run(CliOptions &opts);
private:
    void paint(SkCanvas* canvas, int width, int height);
    void postPaint();
    void drawImGuiVectorCmdsFB(SkCanvas &canvas);

    void loopEmpty(CliOptions const &opts);
    void loopWebp(CliOptions const &opts);
    void loopJpeg(CliOptions const &opts);
    void loopPng(CliOptions const &opts);
    void loopQoi(CliOptions const &opts);
    void loopBmp(CliOptions const &opts);
    void loopFlatbuffers(CliOptions const &opts);
    void loopSvg(CliOptions const &opts);
    void loopSkp(CliOptions const &opts);
    void dispatchUserInteractionEvents();
    static void handleUserInteractionEvent(UserInteractionFB::Event const &ev);

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
    uint64_t fFrame;
    double fPreviousTime;
    rawFrameOutputFormat fOutputFormat;
    FILE *fUserInteractionFH = nullptr;
};