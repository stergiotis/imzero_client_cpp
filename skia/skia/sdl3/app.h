#pragma once

#include <SDL3/SDL_video.h>
#include "include/core/SkSurface.h"
#include "../cliOptions.h"
#include "../vectorCmdSkiaRenderer.h"
#include "../setupUI.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/GrDirectContext.h"

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
    int Run(CliOptions &opts);
private:
    void Paint(SkSurface* surface, int width, int height);
    void DrawImGuiVectorCmdsFB(SkCanvas &canvas);
    void createContext(ImVec4 const &clearColor, int width, int height);
    void destroyContext();
    sk_sp<SkSurface> getSurface();

    sk_sp<SkFontMgr> fFontMgr = nullptr;
    SkPaint fFontPaint;
    VectorCmdSkiaRenderer fVectorCmdSkiaRenderer;
    size_t fTotalVectorCmdSerializedSize = 0;
    size_t fSkpBytesWritten = 0;
    size_t fSvgBytesWritten = 0;
    size_t fPngBytesWritten = 0;
    ImZeroSkiaSetupUI fImZeroSkiaSetupUi;
    SkColor fBackgroundColor;
    bool fFffiInterpreter = false;
    bool fUseVectorCmd = false;

    /* interactive */
    int mainLoopInteractive(CliOptions &opts,SDL_GLContext glContext, ImVec4 const &clearColor);
    sk_sp<const GrGLInterface> fNativeInterface = nullptr;
    sk_sp<GrDirectContext> fContext = nullptr;
    sk_sp<SkSurface> fSurface = nullptr;
    SDL_Window *fWindow = nullptr;

    /* video */
    int mainLoopHeadless(CliOptions &opts, ImVec4 const &clearColor);
    void loopEmpty(CliOptions const &opts);
    void loopWebp(CliOptions const &opts);
    void loopJpeg(CliOptions const &opts);
    void loopPng(CliOptions const &opts);
    void loopQoi(CliOptions const &opts);
    void loopBmp(CliOptions const &opts);
    void loopFlatbuffers(CliOptions const &opts);
    void loopSvg(CliOptions const &opts);
    void loopSkp(CliOptions const &opts);
    void dispatchUserInteractionEventsBinary();
    void dispatchUserInteractionEventsFB();
    static void handleUserInteractionEvent(ImZeroFB::InputEvent const &ev);
    void ensureRawFrameFileOpened(const CliOptions &opts);
    void videoPostPaint();
    void videoPaint(SkCanvas* canvas, int width, int height);

    uint64_t fFrame;
    double fPreviousTime;
    rawFrameOutputFormat fOutputFormat;
    int fUserInteractionFd;
    bool fDispatchInteractionEvents = false;
    bool fInteractionEventsAreInBinary = false;
    bool fInteractionClientConnected = false;
    flatbuffers::FlatBufferBuilder fInteractionFBBuilder{};
    bool fRawFrameFileOpened = true;
    std::unique_ptr<SkFILEWStream> fRawFrameFileStream = nullptr;
    uint64_t fTime = 0;
};