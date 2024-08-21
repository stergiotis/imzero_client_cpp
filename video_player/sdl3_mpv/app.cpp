#include <cstdio>
#include "app.h"
#include "flatbuffers/minireflect.h"

#define HANDLE_MPV_RETURN(errorCode) handleMpvRetr((errorCode),__func__,__FILE__,__LINE__)
#define HANDLE_SDL_RETURN(errorCode) handleSdlRetr((errorCode),__func__,__FILE__,__LINE__)

static UserInteractionFB::KeyCode keycodeFromSdl3Code(SDL_Keycode keycode) {
    switch(keycode) {
        case SDLK_TAB:
            return UserInteractionFB::KeyCode_Key_Tab;
        case SDLK_LEFT:
            return UserInteractionFB::KeyCode_Key_LeftArrow;
        case SDLK_RIGHT:
            return UserInteractionFB::KeyCode_Key_RightArrow;
        case SDLK_UP:
            return UserInteractionFB::KeyCode_Key_UpArrow;
        case SDLK_DOWN:
            return UserInteractionFB::KeyCode_Key_DownArrow;
        case SDLK_PAGEUP:
            return UserInteractionFB::KeyCode_Key_PageUp;
        case SDLK_PAGEDOWN:
            return UserInteractionFB::KeyCode_Key_PageDown;
        case SDLK_HOME:
            return UserInteractionFB::KeyCode_Key_Home;
        case SDLK_END:
            return UserInteractionFB::KeyCode_Key_End;
        case SDLK_INSERT:
            return UserInteractionFB::KeyCode_Key_Insert;
        case SDLK_DELETE:
            return UserInteractionFB::KeyCode_Key_Delete;
        case SDLK_BACKSPACE:
            return UserInteractionFB::KeyCode_Key_Backspace;
        case SDLK_SPACE:
            return UserInteractionFB::KeyCode_Key_Space;
        case SDLK_RETURN:
            return UserInteractionFB::KeyCode_Key_Enter;
        case SDLK_ESCAPE:
            return UserInteractionFB::KeyCode_Key_Escape;
        case SDLK_APOSTROPHE:
            return UserInteractionFB::KeyCode_Key_Apostrophe;
        case SDLK_COMMA:
            return UserInteractionFB::KeyCode_Key_Comma;
        case SDLK_MINUS:
            return UserInteractionFB::KeyCode_Key_Minus;
        case SDLK_PERIOD:
            return UserInteractionFB::KeyCode_Key_Period;
        case SDLK_SLASH:
            return UserInteractionFB::KeyCode_Key_Slash;
        case SDLK_SEMICOLON:
            return UserInteractionFB::KeyCode_Key_Semicolon;
        case SDLK_EQUALS:
            return UserInteractionFB::KeyCode_Key_Equal;
        case SDLK_LEFTBRACKET:
            return UserInteractionFB::KeyCode_Key_LeftBracket;
        case SDLK_BACKSLASH:
            return UserInteractionFB::KeyCode_Key_Backslash;
        case SDLK_RIGHTBRACKET:
            return UserInteractionFB::KeyCode_Key_RightBracket;
        case SDLK_GRAVE:
            return UserInteractionFB::KeyCode_Key_GraveAccent;
        case SDLK_CAPSLOCK:
            return UserInteractionFB::KeyCode_Key_CapsLock;
        case SDLK_SCROLLLOCK:
            return UserInteractionFB::KeyCode_Key_ScrollLock;
        case SDLK_NUMLOCKCLEAR:
            return UserInteractionFB::KeyCode_Key_NumLock;
        case SDLK_PRINTSCREEN:
            return UserInteractionFB::KeyCode_Key_PrintScreen;
        case SDLK_PAUSE:
            return UserInteractionFB::KeyCode_Key_Pause;
        case SDLK_KP_0:
            return UserInteractionFB::KeyCode_Key_Keypad0;
        case SDLK_KP_1:
            return UserInteractionFB::KeyCode_Key_Keypad1;
        case SDLK_KP_2:
            return UserInteractionFB::KeyCode_Key_Keypad2;
        case SDLK_KP_3:
            return UserInteractionFB::KeyCode_Key_Keypad3;
        case SDLK_KP_4:
            return UserInteractionFB::KeyCode_Key_Keypad4;
        case SDLK_KP_5:
            return UserInteractionFB::KeyCode_Key_Keypad5;
        case SDLK_KP_6:
            return UserInteractionFB::KeyCode_Key_Keypad6;
        case SDLK_KP_7:
            return UserInteractionFB::KeyCode_Key_Keypad7;
        case SDLK_KP_8:
            return UserInteractionFB::KeyCode_Key_Keypad8;
        case SDLK_KP_9:
            return UserInteractionFB::KeyCode_Key_Keypad9;
        case SDLK_KP_PERIOD:
            return UserInteractionFB::KeyCode_Key_KeypadDecimal;
        case SDLK_KP_DIVIDE:
            return UserInteractionFB::KeyCode_Key_KeypadDivide;
        case SDLK_KP_MULTIPLY:
            return UserInteractionFB::KeyCode_Key_KeypadMultiply;
        case SDLK_KP_MINUS:
            return UserInteractionFB::KeyCode_Key_KeypadSubtract;
        case SDLK_KP_PLUS:
            return UserInteractionFB::KeyCode_Key_KeypadAdd;
        case SDLK_KP_ENTER:
            return UserInteractionFB::KeyCode_Key_KeypadEnter;
        case SDLK_KP_EQUALS:
            return UserInteractionFB::KeyCode_Key_KeypadEqual;
        case SDLK_LCTRL:
            return UserInteractionFB::KeyCode_Key_LeftCtrl;
        case SDLK_LSHIFT:
            return UserInteractionFB::KeyCode_Key_LeftShift;
        case SDLK_LALT:
            return UserInteractionFB::KeyCode_Key_LeftAlt;
        case SDLK_LGUI:
            return UserInteractionFB::KeyCode_Key_LeftSuper;
        case SDLK_RCTRL:
            return UserInteractionFB::KeyCode_Key_RightCtrl;
        case SDLK_RSHIFT:
            return UserInteractionFB::KeyCode_Key_RightShift;
        case SDLK_RALT:
            return UserInteractionFB::KeyCode_Key_RightAlt;
        case SDLK_RGUI:
            return UserInteractionFB::KeyCode_Key_RightSuper;
        case SDLK_APPLICATION:
            return UserInteractionFB::KeyCode_Key_Menu;
        case SDLK_0:
            return UserInteractionFB::KeyCode_Key_0;
        case SDLK_1:
            return UserInteractionFB::KeyCode_Key_1;
        case SDLK_2:
            return UserInteractionFB::KeyCode_Key_2;
        case SDLK_3:
            return UserInteractionFB::KeyCode_Key_3;
        case SDLK_4:
            return UserInteractionFB::KeyCode_Key_4;
        case SDLK_5:
            return UserInteractionFB::KeyCode_Key_5;
        case SDLK_6:
            return UserInteractionFB::KeyCode_Key_6;
        case SDLK_7:
            return UserInteractionFB::KeyCode_Key_7;
        case SDLK_8:
            return UserInteractionFB::KeyCode_Key_8;
        case SDLK_9:
            return UserInteractionFB::KeyCode_Key_9;
        case SDLK_a:
            return UserInteractionFB::KeyCode_Key_A;
        case SDLK_b:
            return UserInteractionFB::KeyCode_Key_B;
        case SDLK_c:
            return UserInteractionFB::KeyCode_Key_C;
        case SDLK_d:
            return UserInteractionFB::KeyCode_Key_D;
        case SDLK_e:
            return UserInteractionFB::KeyCode_Key_E;
        case SDLK_f:
            return UserInteractionFB::KeyCode_Key_F;
        case SDLK_g:
            return UserInteractionFB::KeyCode_Key_G;
        case SDLK_h:
            return UserInteractionFB::KeyCode_Key_H;
        case SDLK_i:
            return UserInteractionFB::KeyCode_Key_I;
        case SDLK_j:
            return UserInteractionFB::KeyCode_Key_J;
        case SDLK_k:
            return UserInteractionFB::KeyCode_Key_K;
        case SDLK_l:
            return UserInteractionFB::KeyCode_Key_L;
        case SDLK_m:
            return UserInteractionFB::KeyCode_Key_M;
        case SDLK_n:
            return UserInteractionFB::KeyCode_Key_N;
        case SDLK_o:
            return UserInteractionFB::KeyCode_Key_O;
        case SDLK_p:
            return UserInteractionFB::KeyCode_Key_P;
        case SDLK_q:
            return UserInteractionFB::KeyCode_Key_Q;
        case SDLK_r:
            return UserInteractionFB::KeyCode_Key_R;
        case SDLK_s:
            return UserInteractionFB::KeyCode_Key_S;
        case SDLK_t:
            return UserInteractionFB::KeyCode_Key_T;
        case SDLK_u:
            return UserInteractionFB::KeyCode_Key_U;
        case SDLK_v:
            return UserInteractionFB::KeyCode_Key_V;
        case SDLK_w:
            return UserInteractionFB::KeyCode_Key_W;
        case SDLK_x:
            return UserInteractionFB::KeyCode_Key_X;
        case SDLK_y:
            return UserInteractionFB::KeyCode_Key_Y;
        case SDLK_z:
            return UserInteractionFB::KeyCode_Key_Z;
        case SDLK_F1:
            return UserInteractionFB::KeyCode_Key_F1;
        case SDLK_F2:
            return UserInteractionFB::KeyCode_Key_F2;
        case SDLK_F3:
            return UserInteractionFB::KeyCode_Key_F3;
        case SDLK_F4:
            return UserInteractionFB::KeyCode_Key_F4;
        case SDLK_F5:
            return UserInteractionFB::KeyCode_Key_F5;
        case SDLK_F6:
            return UserInteractionFB::KeyCode_Key_F6;
        case SDLK_F7:
            return UserInteractionFB::KeyCode_Key_F7;
        case SDLK_F8:
            return UserInteractionFB::KeyCode_Key_F8;
        case SDLK_F9:
            return UserInteractionFB::KeyCode_Key_F9;
        case SDLK_F10:
            return UserInteractionFB::KeyCode_Key_F10;
        case SDLK_F11:
            return UserInteractionFB::KeyCode_Key_F11;
        case SDLK_F12:
            return UserInteractionFB::KeyCode_Key_F12;
        case SDLK_F13:
            return UserInteractionFB::KeyCode_Key_F13;
        case SDLK_F14:
            return UserInteractionFB::KeyCode_Key_F14;
        case SDLK_F15:
            return UserInteractionFB::KeyCode_Key_F15;
        case SDLK_F16:
            return UserInteractionFB::KeyCode_Key_F16;
        case SDLK_F17:
            return UserInteractionFB::KeyCode_Key_F17;
        case SDLK_F18:
            return UserInteractionFB::KeyCode_Key_F18;
        case SDLK_F19:
            return UserInteractionFB::KeyCode_Key_F19;
        case SDLK_F20:
            return UserInteractionFB::KeyCode_Key_F20;
        case SDLK_F21:
            return UserInteractionFB::KeyCode_Key_F21;
        case SDLK_F22:
            return UserInteractionFB::KeyCode_Key_F22;
        case SDLK_F23:
            return UserInteractionFB::KeyCode_Key_F23;
        case SDLK_F24:
            return UserInteractionFB::KeyCode_Key_F24;
        case SDLK_AC_BACK:
            return UserInteractionFB::KeyCode_Key_AppBack;
        case SDLK_AC_FORWARD:
            return UserInteractionFB::KeyCode_Key_AppForward;
        default:
            return UserInteractionFB::KeyCode_Key_None;
    }
}

static void *getProAddressMpv(void *fn_ctx, const char *name) {
    return reinterpret_cast<void *>(SDL_GL_GetProcAddress(name));
}

static void onMpvEvent(void *ctx) {
    SDL_Event event = {.type = reinterpret_cast<App*>(ctx)->fWakeupOnMpvEvents};
    SDL_PushEvent(&event);
}

static void onMpvRenderUpdate(void *ctx) {
    SDL_Event event = {.type = reinterpret_cast<App*>(ctx)->fWakeupOnMpvRenderUpdate};
    SDL_PushEvent(&event);
}

App::~App() {
    teardown();
}
void App::serializeUserInteractionEventFB(const uint8_t *&out,size_t &size, flatbuffers::Offset<UserInteractionFB::Event> e) {
    fFlatBufferBuilder.FinishSizePrefixed(e);
    size = fFlatBufferBuilder.GetSize();
    out = fFlatBufferBuilder.GetBufferPointer();

    {
        auto txt = flatbuffers::FlatBufferToString(out+4,UserInteractionFB::EventTypeTable());
        fprintf(stderr, "userInteractionEvent=%s\n", txt.c_str());
    }
}
void App::sendUserInteractionEvent(UserInteractionFB::UserInteraction eventType, flatbuffers::Offset<void> e) {
    const uint8_t *ptr = nullptr;
    size_t sz = 0;
    serializeUserInteractionEventFB(ptr,sz,UserInteractionFB::CreateEvent(fFlatBufferBuilder, eventType, e));
    fwrite(ptr,1,sz,fUserInteractionOutput);
    fflush(fUserInteractionOutput);
}

void App::handleSdlEvent(SDL_Event &event) {
    fFlatBufferBuilder.Clear();
    switch(event.type) {
        case SDL_EVENT_MOUSE_MOTION:
        {
            auto const ev = event.motion;
            auto const pos = UserInteractionFB::SingleVec2(ev.x,ev.y);
            auto const e = UserInteractionFB::CreateEventMouseMotion(fFlatBufferBuilder,&pos,ev.which,ev.which == SDL_TOUCH_MOUSEID);
            sendUserInteractionEvent(UserInteractionFB::UserInteraction_EventMouseMotion, e.Union());
        }
        break;
        case SDL_EVENT_MOUSE_WHEEL:
        {
            auto const ev = event.wheel;
            auto const pos = UserInteractionFB::SingleVec2(ev.x,ev.y);
            auto const e = UserInteractionFB::CreateEventMouseWheel(fFlatBufferBuilder, &pos, ev.which, ev.which == SDL_TOUCH_MOUSEID);
            sendUserInteractionEvent(UserInteractionFB::UserInteraction_EventMouseWheel, e.Union());
        }
        break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN: // fallthrough
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            auto const ev = event.button;
            UserInteractionFB::MouseButton b = UserInteractionFB::MouseButton_None;
            switch(ev.button) {
                case SDL_BUTTON_LEFT:
                    b = UserInteractionFB::MouseButton_Left;
                    break;
                case SDL_BUTTON_RIGHT:
                    b = UserInteractionFB::MouseButton_Right;
                    break;
                case SDL_BUTTON_MIDDLE:
                    b = UserInteractionFB::MouseButton_Middle;
                    break;
                case SDL_BUTTON_X1:
                    b = UserInteractionFB::MouseButton_X1;
                    break;
                case SDL_BUTTON_X2:
                    b = UserInteractionFB::MouseButton_X2;
                    break;
            }
            auto const pos = UserInteractionFB::SingleVec2(ev.x,ev.y);
            UserInteractionFB::MouseButtonEventType t = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? UserInteractionFB::MouseButtonEventType_Down : UserInteractionFB::MouseButtonEventType_Up;
            auto const e = UserInteractionFB::CreateEventMouseButton(fFlatBufferBuilder,&pos,ev.which,ev.which == SDL_TOUCH_MOUSEID,b,t);
            sendUserInteractionEvent(UserInteractionFB::UserInteraction_EventMouseButton, e.Union());
        }
        break;
        case SDL_EVENT_TEXT_INPUT:
        {
            auto const ev = event.text;
            auto const t = fFlatBufferBuilder.CreateString(ev.text);
            auto const e = UserInteractionFB::CreateEventTextInput(fFlatBufferBuilder,t);
            sendUserInteractionEvent(UserInteractionFB::UserInteraction_EventTextInput, e.Union());
        }
        break;
        case SDL_EVENT_KEY_DOWN: // fallthrough
        case SDL_EVENT_KEY_UP:
        {
            auto const ev = event.key;
            auto const keyModSdl = ev.keysym.mod;
            auto mod = UserInteractionFB::KeyModifiers_None;
            mod = static_cast<UserInteractionFB::KeyModifiers>(static_cast<uint8_t>(mod) | static_cast<uint8_t>((keyModSdl & SDL_KMOD_CTRL) ? UserInteractionFB::KeyModifiers_Ctrl : UserInteractionFB::KeyModifiers_None));
            mod = static_cast<UserInteractionFB::KeyModifiers>(static_cast<uint8_t>(mod) | static_cast<uint8_t>((keyModSdl & SDL_KMOD_SHIFT) ? UserInteractionFB::KeyModifiers_Shift : UserInteractionFB::KeyModifiers_None));
            mod = static_cast<UserInteractionFB::KeyModifiers>(static_cast<uint8_t>(mod) | static_cast<uint8_t>((keyModSdl & SDL_KMOD_ALT) ? UserInteractionFB::KeyModifiers_Alt : UserInteractionFB::KeyModifiers_None));
            mod = static_cast<UserInteractionFB::KeyModifiers>(static_cast<uint8_t>(mod) | static_cast<uint8_t>((keyModSdl & SDL_KMOD_GUI) ? UserInteractionFB::KeyModifiers_Super : UserInteractionFB::KeyModifiers_None));
            auto const e = UserInteractionFB::CreateEventKeyboard(fFlatBufferBuilder, mod, keycodeFromSdl3Code(ev.keysym.sym), event.type == SDL_EVENT_KEY_DOWN,ev.keysym.sym,ev.keysym.scancode);
            sendUserInteractionEvent(UserInteractionFB::UserInteraction_EventKeyboard,e.Union());
        }
        break;
        //case SDL_EVENT_KEY_DOWN:
        //case SDL_EVENT_KEY_UP:
        //{
        //    ImGui_ImplSDL3_UpdateKeyModifiers((SDL_Keymod)event->key.keysym.mod);
        //    ImGuiKey key = ImGui_ImplSDL3_KeycodeToImGuiKey(event->key.keysym.sym);
        //    io.AddKeyEvent(key, (event->type == SDL_EVENT_KEY_DOWN));
        //    io.SetKeyEventNativeData(key, event->key.keysym.sym, event->key.keysym.scancode, event->key.keysym.scancode); // To support legacy indexing (<1.87 user code). Legacy backend uses SDLK_*** as indices to IsKeyXXX() functions.
        //    return true;
        //}
    }
}

const char *App::step(bool &quit) {
    SDL_Event event;
    if(SDL_WaitEvent(&event) != 1) {
        return "event loop error";
    }
    quit = false;

    int redrawNeeded = 0;
    switch (event.type) {
        case SDL_EVENT_QUIT:
            quit = true;
            break;
        case SDL_EVENT_WINDOW_EXPOSED:
            redrawNeeded = 1;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (event.key.keysym.sym == SDLK_SPACE) {
                const char *cmd_pause[] = {"cycle", "pause", nullptr};
                mpv_command_async(fMpvHandle, 0, cmd_pause);
            }
            if (event.key.keysym.sym == SDLK_s) {
                // Also requires MPV_RENDER_PARAM_ADVANCED_CONTROL if you want
                // screenshots to be rendered on GPU (like --vo=gpu would do).
                const char *cmd_scr[] = {"screenshot-to-file",
                                         "screenshot.png",
                                         "window",
                                         NULL};
                printf("attempting to save screenshot to %s\n", cmd_scr[1]);
                mpv_command_async(fMpvHandle, 0, cmd_scr);
            }
            handleSdlEvent(event);
            break;
        default:
            // Happens when there is new work for the render thread (such as
            // rendering a new video frame or redrawing it).
            if (event.type == fWakeupOnMpvRenderUpdate) {
                uint64_t flags = mpv_render_context_update(fMpvRenderContext);
                if (flags & MPV_RENDER_UPDATE_FRAME) {
                    redrawNeeded = 1;
                }
            } else if (event.type == fWakeupOnMpvEvents) {
                // Happens when at least 1 new event is in the mpv event queue.
                processMpvEvents();
            } else {
                handleSdlEvent(event);
            }
    }
    if(redrawNeeded) {
       redraw();
    }
    return nullptr;
}

bool App::scheduleMpvCommandAsync2(const char *command, const char *arg1) {
    const char *cmd[] = {command, arg1, nullptr};
    return !HANDLE_MPV_RETURN(mpv_command_async(fMpvHandle, 0, cmd));
}
bool App::scheduleMpvCommand2(const char *command, const char *arg1) {
    const char *cmd[] = {command, arg1, nullptr};
    return !HANDLE_MPV_RETURN(mpv_command(fMpvHandle, cmd));
}
bool App::scheduleMpvCommandAsync1(const char *command) {
    const char *cmd[] = {command, nullptr};
    return !HANDLE_MPV_RETURN(mpv_command_async(fMpvHandle, 0, cmd));
}

bool App::handleMpvRetr(int errorCode, const char *func, const char *filename, int line) {
    if(errorCode >= 0) {
        return false;
    }
    fprintf(stderr, "mpv error: %s. %s %s:%d\n", mpv_error_string(errorCode), func, filename, line);
    return true;
}

bool App::handleSdlRetr(int errorCode, const char *func, const char *filename, int line) {
    if(errorCode >= 0) {
        return false;
    }
    fprintf(stderr, "sdl error: %s. %s %s:%d\n", SDL_GetError(), func, filename, line);
    return true;
}

// FIXME allow multiple calls to setup?
const char *App::setup(const char *inputFile, FILE *userInteractionOutput) {
    teardown();
    fUserInteractionOutput = userInteractionOutput;
    fMpvHandle = mpv_create();

    if(fMpvHandle == nullptr) {
        return "unable to create mpv handle";
    }
    // NOTE: this option needs to be set before mpv_initialize
    if(HANDLE_MPV_RETURN(mpv_set_option_string(fMpvHandle, "vo", "libmpv"))) {
        return "unable to set option 'vo'";
    }
    if(HANDLE_MPV_RETURN(mpv_initialize(fMpvHandle))) {
        return "unable to initialize mpv";
    }

    if(HANDLE_MPV_RETURN(mpv_request_log_messages(fMpvHandle, "debug"))) {
        return "unable to set mpv loglevel";
    }

    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

    if(HANDLE_SDL_RETURN(SDL_InitSubSystem(SDL_INIT_VIDEO))) {
        return "unable to initialize sdl";
    }

    if(fSdlWindow == nullptr) {
        fSdlWindow = SDL_CreateWindow("imzero sdl2 mpv video player", 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if(!fSdlWindow) {
            return "unable to create sdl window";
        }
    }

    if(fSdlGlContext == nullptr) {
        fSdlGlContext = SDL_GL_CreateContext(fSdlWindow);
        if(!fSdlGlContext) {
            return "unable to create sdl gl context";
        }
    }

    mpv_opengl_init_params initParams{
            .get_proc_address = getProAddressMpv
    };
    int one = 1;
    mpv_render_param params[] = {
            {MPV_RENDER_PARAM_API_TYPE, reinterpret_cast<void*>(const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL))},
            {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &initParams},
            // Tell libmpv that you will call mpv_render_context_update() on render
            // context update callbacks, and that you will _not_ block on the core
            // ever (see <libmpv/render.h> "Threading" section for what libmpv
            // functions you can call at all when this is active).
            // In particular, this means you must call e.g. mpv_command_async()
            // instead of mpv_command().
            // If you want to use synchronous calls, either make them on a separate
            // thread, or remove the option below (this will disable features like
            // DR and is not recommended anyway).
            {MPV_RENDER_PARAM_ADVANCED_CONTROL, &one},
            {static_cast<mpv_render_param_type>(0)}
    };

    // This makes mpv use the currently set GL context. It will use the callback
    // (passed via params) to resolve GL builtin functions, as well as extensions.
    if(HANDLE_MPV_RETURN(mpv_render_context_create(&fMpvRenderContext, fMpvHandle, params))) {
        return "unable to initialize mpv gl context";
    }

    // We use events for thread-safe notification of the SDL main loop.
    // Generally, the wakeup callbacks (set further below) should do as least
    // work as possible, and merely wake up another thread to do actual work.
    // On SDL, waking up the mainloop is the ideal course of action. SDL's
    // SDL_PushEvent() is thread-safe, so we use that.
    fWakeupOnMpvRenderUpdate = SDL_RegisterEvents(1);
    fWakeupOnMpvEvents = SDL_RegisterEvents(1);
    if(fWakeupOnMpvRenderUpdate == static_cast<Uint32>(-1) ||
       fWakeupOnMpvEvents == static_cast<Uint32>(-1)) {
        return "unable to register events";
    }

    // When normal mpv events are available.
    mpv_set_wakeup_callback(fMpvHandle, onMpvEvent, this);

    // When there is a need to call mpv_render_context_update(), which can
    // request a new frame to be rendered.
    // (Separate from the normal event handling mechanism for the sake of
    //  users which run OpenGL on a different thread.)
    mpv_render_context_set_update_callback(fMpvRenderContext, onMpvRenderUpdate, this);

    /*
    if(!scheduleMpvCommandAsync1("keep-open")) {
        return "unable to schedule command";
    }
    if(!scheduleMpvCommandAsync1("cursor-autohide-fs-only")) {
        return "unable to schedule command";
    }
    if(!scheduleMpvCommand2("profile","low-latency")) {
        return "unable to schedule profile command";
    }
    */
    {
        const char *optionsString[] = {"profile", "low-latency",
                                       "cache", "no",
                                       "demuxer-thread", "no",
                                       "correct-pts", "no",
                                       "keep-open", "always",
                                       "cursor-autohide", "no",
                //                       "vo","x11",
               // "vo","xv",
                                       "idle","yes",
                                       //"fs","yes",
                //                       "force-window","yes",
                                       nullptr};
        for(int i=0; optionsString[i] != nullptr; i+=2) {
            if(HANDLE_MPV_RETURN(mpv_set_option_string(fMpvHandle, optionsString[i], optionsString[i + 1]))) {
                return "unable to set option";
            }
        }
        const char *optionsBool[] = {"untimed", "1",
                                       nullptr};
        for(int i=0; optionsBool[i] != nullptr; i+=2) {
            int v = optionsBool[i + 1][0] == '1';
            if(HANDLE_MPV_RETURN(mpv_set_option(fMpvHandle, optionsBool[i], MPV_FORMAT_FLAG, &v))) {
                return "unable to set option";
            }
        }
    }

    if(!scheduleMpvCommandAsync2("loadfile", inputFile)) {
        return "unable to schedule loadfile command";
    }

    return nullptr;
}

void App::redraw() {
    int w, h;
    SDL_GetWindowSize(fSdlWindow, &w, &h);
    mpv_opengl_fbo fbo{
            .fbo = 0,
            .w = w,
            .h = h,
    };
    int one = 1;
    mpv_render_param params[] = {
            // Specify the default framebuffer (0) as target. This will
            // render onto the entire screen. If you want to show the video
            // in a smaller rectangle or apply fancy transformations, you'll
            // need to render into a separate FBO and draw it manually.
            {MPV_RENDER_PARAM_OPENGL_FBO, &fbo},
            // Flip rendering (needed due to flipped GL coordinate system).
            {MPV_RENDER_PARAM_FLIP_Y, &one},
            {static_cast<mpv_render_param_type>(0)}
    };
    // See render_gl.h on what OpenGL environment mpv expects, and
    // other API details.
    mpv_render_context_render(fMpvRenderContext, params);
    SDL_GL_SwapWindow(fSdlWindow);
}

void App::processMpvEvents() {
    while(true) {
        mpv_event *mp_event = mpv_wait_event(fMpvHandle, 0);
        switch(mp_event->event_id) {
            case MPV_EVENT_NONE:
                return;
            case MPV_EVENT_LOG_MESSAGE:
                {

                    auto const msg = static_cast<mpv_event_log_message*>(mp_event->data);
                    fprintf(stderr, "mpv log: [%s] %s %s",
                            msg->level,
                            msg->prefix,
                            msg->text);
                }
                break;
            default:
                fprintf(stderr, "received mpv event %s\n", mpv_event_name(mp_event->event_id));
        }
    }
}

void App::teardown() {
    if(fMpvRenderContext != nullptr) {
        // Destroy the GL renderer and all the GL objects it allocated. If video
        // is still running, the video track will be deselected.
        mpv_render_context_free(fMpvRenderContext);
        fMpvRenderContext = nullptr;
    }

    if(fMpvHandle != nullptr) {
        mpv_destroy(fMpvHandle);
        fMpvHandle = nullptr;
    }

    fUserInteractionOutput = nullptr;
}
