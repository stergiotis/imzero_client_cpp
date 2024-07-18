#pragma once

#include <mpv/client.h>
#include <mpv/render_gl.h>
#include <SDL3/SDL.h>

class App {
public:
    App() = default;
    ~App();

    // @return nullptr on success; error message on failure
    const char *setup(const char *input_file);
    // @return nullptr on success; error message on failure
    const char *step(bool &quit);

public: // needed in event handlers
    Uint32 wakeup_on_mpv_render_update = 0;
    Uint32 wakeup_on_mpv_events = 0;

private:
    void redraw();
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
};