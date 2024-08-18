#pragma once

#include <SDL3/SDL_video.h>
#include "include/core/SkSurface.h"
#include "../cliOptions.h"
#include "../vectorCmdSkiaRenderer.h"
#include "../setupUI.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/GrDirectContext.h"

class App {
public:
    App();
    ~App() = default;
    int Run(CliOptions &opts);
private:
    void Paint(SkSurface* surface, int width, int height);
    void DrawImGuiVectorCmdsFB(SkCanvas &canvas);
    void createContext(ImVec4 const &clearColor, int width, int height);
    void destroyContext();
    sk_sp<SkSurface> getSurface();

    sk_sp<const GrGLInterface> fNativeInterface = nullptr;
    sk_sp<GrDirectContext> fContext = nullptr;
    sk_sp<SkSurface> fSurface = nullptr;
    SDL_Window *fWindow = nullptr;

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
};