#pragma once

#include <mpv/client.h>
#include <mpv/render_gl.h>
#include <SDL3/SDL.h>
#include "flatbuffers/flatbuffers.h"
#include "../src/userInteraction_generated.h"

class App {
public:
    App() = default;
    ~App();

    // @return nullptr on success; error message on failure
    const char *setup(const char *inputFile,FILE *userInteractionOutput);
    // @return nullptr on success; error message on failure
    const char *step(bool &quit);

public: // needed in event handlers
    Uint32 fWakeupOnMpvRenderUpdate = 0;
    Uint32 fWakeupOnMpvEvents = 0;

private:
    void serializeUserInteractionEventFB(const uint8_t *&out,size_t &size, flatbuffers::Offset<void> e);
    void sendUserInteractionEvent(flatbuffers::Offset<void> e);
    void redraw();
    void teardown();
    void handleSdlEvent(SDL_Event &event);
    void processMpvEvents();
    // @return true on success
    bool scheduleMpvCommandAsync2(const char *command, const char *arg1);
    // @return true on failure
    static bool handleMpvRetr(int errorCode, const char *func, const char *filename, int line);
    // @return true on failure
    static bool handleSdlRetr(int errorCode, const char *func, const char *filename, int line);

    mpv_handle *fMpvHandle = nullptr;
    mpv_render_context *fMpvRenderContext = nullptr;
    SDL_GLContext fSdlGlContext = nullptr;
    SDL_Window *fSdlWindow = nullptr;
    flatbuffers::FlatBufferBuilder fFlatBufferBuilder;
    FILE *fUserInteractionOutput = nullptr;
};