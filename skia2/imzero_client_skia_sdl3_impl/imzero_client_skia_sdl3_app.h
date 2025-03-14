#pragma once

#include <ImZeroFB.out.h>
#include <include/core/SkStream.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_video.h>
#include "include/core/SkSurface.h"
#include "imzero_client_skia_sdl3_cli_options.h"
#include "include/gpu/GrDirectContext.h"
#include "imgui.h"
#include "imgui_skia_app_sdl3.h"

namespace ImZeroClient
{
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
    rawFrameOutputFormat resolveRawFrameOutputFormat(const char *name);
    ImZeroFB::KeyCode sdlKeyCodeToImZeroFBKeyCode(SDL_Keycode keyCode,SDL_Scancode scanCode);
    ImGuiKey imZeroFBKeyCodeToImGuiKey(ImZeroFB::KeyCode keyCode);

    class App {
    public:
        App();
        ~App();
        int Run(ImZeroCliOptions &opts);
    private:
        sk_sp<SkSurface> getSurfaceRaster(int w, int h);
        bool fFffiInterpreter = false;

        /* video */
        int mainLoopHeadless(const ImZeroCliOptions &opts, ImVec4 const &clearColor);
        void loopEmpty(ImZeroCliOptions const &opts);
        void loopWebp(ImZeroCliOptions const &opts);
        void loopJpeg(ImZeroCliOptions const &opts);
        void loopPng(ImZeroCliOptions const &opts);
        void loopQoi(ImZeroCliOptions const &opts);
        void loopBmp(ImZeroCliOptions const &opts);
        void loopFlatbuffers(ImZeroCliOptions const &opts);
        void loopSvg(ImZeroCliOptions const &opts);
        void loopSkp(ImZeroCliOptions const &opts);
        void dispatchUserInteractionEventsBinary();
        void dispatchUserInteractionEventsFB();
        static void handleUserInteractionEvent(ImZeroFB::InputEvent const &ev);
        void ensureRawFrameFileOpened(const ImZeroCliOptions &opts);
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

        ImGuiSkia::Driver::App fApp{};
        sk_sp<SkSurface> fSurface{nullptr};
    };
}