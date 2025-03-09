#pragma once

#include <SDL3/SDL_video.h>
#include "include/core/SkSurface.h"
#include "imgui_skia_cli_options.h"
#include "imgui_skia_imzero_cmd_render.h"
#include "imgui_skia_setup_ui.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/GrDirectContext.h"

namespace ImGuiSkia::Driver {
    class App {
    public:
        App();
        void postRender(FrameExportFormatE frameExportFormat, SkSurface* surface, int width, int height);
        ~App();
        void prePaint(const SkSurface* surface, int width, int height);
        SkSurface* preRender(bool& done, int& width, int& height);
        FrameExportFormatE render(SkSurface* surface, int width, int height);
        void postPaint(SkSurface* surface, FrameExportFormatE frameExportFormat, int width, int height);

        void setup(CliOptions &opts);
        int mainLoop();
        void cleanup();

    protected:
        void drawImGuiVectorCmdsFB(SkCanvas &canvas);
        void createContext(int width, int height);
        void destroyContext();
        sk_sp<SkSurface> getSurfaceGL();
        sk_sp<SkSurface> getSurfaceRaster(int w, int h);

        SkString fExportBasePath{};

        sk_sp<SkFontMgr> fFontMgr = nullptr;
        SkPaint fFontPaint;
        VectorCmdSkiaRenderer fVectorCmdSkiaRenderer;
        size_t fTotalVectorCmdSerializedSize = 0;
        size_t fSkpBytesWritten = 0;
        size_t fFbBytesWritten = 0;
        size_t fSvgBytesWritten = 0;
        size_t fPngBytesWritten = 0;
        size_t fJpegBytesWritten = 0;
        SetupUI fImZeroSkiaSetupUi;
        SkColor fBackgroundColor;
        bool fUseVectorCmd = false;
        ImVec4 fClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        SDL_GLContext fGlContext = nullptr;

        sk_sp<const GrGLInterface> fNativeInterface = nullptr;
        sk_sp<GrDirectContext> fContext = nullptr;
        sk_sp<SkSurface> fSurface = nullptr;
        SDL_Window *fWindow = nullptr;
    };
}