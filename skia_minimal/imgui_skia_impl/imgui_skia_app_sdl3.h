#pragma once

#include <SDL3/SDL_video.h>
#include "include/core/SkSurface.h"
#include "imgui_skia_cli_options.h"
#include "imgui_skia_imzero_cmd_render.h"
#include "imgui_skia_setup_ui.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/GrDirectContext.h"

class App {
public:
    App();
    virtual ~App();
    int Run(CliOptions &opts);

protected:
	virtual void render();
    void paint(SkSurface* surface, int width, int height);
    void drawImGuiVectorCmdsFB(SkCanvas &canvas);
    void createContext(ImVec4 const &clearColor, int width, int height);
    void destroyContext();
    sk_sp<SkSurface> getSurfaceGL();
    sk_sp<SkSurface> getSurfaceRaster(int w, int h);

    sk_sp<SkFontMgr> fFontMgr{nullptr};
    SkPaint fFontPaint;
    VectorCmdSkiaRenderer fVectorCmdSkiaRenderer;
    size_t fTotalVectorCmdSerializedSize = 0;
    size_t fSkpBytesWritten = 0;
    size_t fSvgBytesWritten = 0;
    size_t fPngBytesWritten = 0;
    SetupUI fImZeroSkiaSetupUi;
    SkColor fBackgroundColor;
    bool fUseVectorCmd = false;

    int mainLoop(CliOptions &opts,SDL_GLContext glContext, ImVec4 const &clearColor);
    sk_sp<const GrGLInterface> fNativeInterface{nullptr};
    sk_sp<GrDirectContext> fContext{nullptr};
    sk_sp<SkSurface> fSurface{nullptr};
    SDL_Window *fWindow = nullptr;
};