#include <fcntl.h>
#include <csignal>
#include "imzero_client_skia_sdl3_app.h"

#include <cstdio>
#include <cstring>


#include <SDL3/SDL_main.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL.h>

#endif

#include "include/core/SkGraphics.h"
#include "include/ports/SkFontMgr_data.h"
#if defined(SK_FONTMGR_FONTCONFIG_AVAILABLE)
#include "include/ports/SkFontMgr_fontconfig.h"
#endif
#if defined(SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE)
#include "include/ports/SkFontMgr_directory.h"
#endif
#include "include/core/SkSpan.h"
#include "include/svg/SkSVGCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/encode/SkPngEncoder.h"
#include "include/encode/SkJpegEncoder.h"
#include "include/encode/SkWebpEncoder.h"
#include "bmp_encoder.h"
#if defined(__linux__)
#include "include/gpu/gl/glx/GrGLMakeGLXInterface.h"
#endif

#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "src/gpu/ganesh/gl/GrGLUtil.h"
//#include "SkBitmap.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkPicture.h"

#include <cassert>

#include "tracy/Tracy.hpp"

#include "render.h"

#include "marshalling/receive.h"
#include "marshalling/send.h"

#include "flatbuffers/minireflect.h"
#include "flatbuffers/util.h"
#include "ImZeroFB.out.h"

#define QOI_IMPLEMENTATION
#define QOI_NO_STDIO
#define QOI_FREE static_assert(false && "free should never be called")
#define QOI_MALLOC(sz) qoiMalloc(sz)
static void *qoiMalloc(size_t sz) {
    static void *qoiBuffer = nullptr;
    static size_t lastSz = 0;
    if(qoiBuffer == nullptr) {
        lastSz = sz;
        qoiBuffer = malloc(sz);
    }
    assert(lastSz == sz && "assuming constant width, height and depth of qoi images");
    return qoiBuffer;
}
#include "../contrib/qoi/qoi.h"

template <typename T>
static void applyFlag(int &flag,T val,bool v) {
    if(v) {
        flag |= val;
    } else {
        flag &= ~(val);
    }
}
constexpr int msaaSampleCount = 0; //4;
constexpr int stencilBits = 8;  // Skia needs 8 stencil bits

ImZeroClient::rawFrameOutputFormat ImZeroClient::resolveRawFrameOutputFormat(const char *name) {
    if(strcmp(name, "qoi") == 0) {
        return kRawFrameOutputFormat_Qoi;
    }
    if(strcmp(name, "webp_lossless") == 0) {
        return kRawFrameOutputFormat_WebP_Lossless;
    }
    if(strcmp(name, "webp_lossy") == 0) {
        return kRawFrameOutputFormat_WebP_Lossy;
    }
    if(strcmp(name, "jpeg") == 0) {
        return kRawFrameOutputFormat_Jpeg;
    }
    if(strcmp(name, "bmp_bgra8888") == 0) {
        return kRawFrameOutputFormat_Bmp_Bgra8888;
    }
    if(strcmp(name, "flatbuffers") == 0) {
        return kRawFrameOutputFormat_Flatbuffers;
    }
    if(strcmp(name, "jpeg") == 0) {
        return kRawFrameOutputFormat_Jpeg;
    }
    if(strcmp(name, "png") == 0) {
        return kRawFrameOutputFormat_Png;
    }
    if(strcmp(name, "skp") == 0) {
        return kRawFrameOutputFormat_Skp;
    }
    if(strcmp(name, "svg") == 0) {
        return kRawFrameOutputFormat_Svg;
    }
    if(strcmp(name, "svg_textaspath") == 0) {
        return kRawFrameOutputFormat_Svg_TextAsPath;
    }
    fprintf(stderr, "unhandled output format %s. disabling output.\n", name);
    return kRawFrameOutputFormat_None;
}
ImZeroFB::KeyCode ImZeroClient::sdlKeyCodeToImZeroFBKeyCode(const SDL_Keycode keyCode,SDL_Scancode scanCode) {
    switch(keyCode) {
        case SDLK_TAB:
            return ImZeroFB::KeyCode_Key_Tab;
        case SDLK_LEFT:
            return ImZeroFB::KeyCode_Key_LeftArrow;
        case SDLK_RIGHT:
            return ImZeroFB::KeyCode_Key_RightArrow;
        case SDLK_UP:
            return ImZeroFB::KeyCode_Key_UpArrow;
        case SDLK_DOWN:
            return ImZeroFB::KeyCode_Key_DownArrow;
        case SDLK_PAGEUP:
            return ImZeroFB::KeyCode_Key_PageUp;
        case SDLK_PAGEDOWN:
            return ImZeroFB::KeyCode_Key_PageDown;
        case SDLK_HOME:
            return ImZeroFB::KeyCode_Key_Home;
        case SDLK_END:
            return ImZeroFB::KeyCode_Key_End;
        case SDLK_INSERT:
            return ImZeroFB::KeyCode_Key_Insert;
        case SDLK_DELETE:
            return ImZeroFB::KeyCode_Key_Delete;
        case SDLK_BACKSPACE:
            return ImZeroFB::KeyCode_Key_Backspace;
        case SDLK_SPACE:
            return ImZeroFB::KeyCode_Key_Space;
        case SDLK_RETURN:
            return ImZeroFB::KeyCode_Key_Enter;
        case SDLK_ESCAPE:
            return ImZeroFB::KeyCode_Key_Escape;
        case SDLK_APOSTROPHE:
            return ImZeroFB::KeyCode_Key_Apostrophe;
        case SDLK_COMMA:
            return ImZeroFB::KeyCode_Key_Comma;
        case SDLK_MINUS:
            return ImZeroFB::KeyCode_Key_Minus;
        case SDLK_PERIOD:
            return ImZeroFB::KeyCode_Key_Period;
        case SDLK_SLASH:
            return ImZeroFB::KeyCode_Key_Slash;
        case SDLK_SEMICOLON:
            return ImZeroFB::KeyCode_Key_Semicolon;
        case SDLK_EQUALS:
            return ImZeroFB::KeyCode_Key_Equal;
        case SDLK_LEFTBRACKET:
            return ImZeroFB::KeyCode_Key_LeftBracket;
        case SDLK_BACKSLASH:
            return ImZeroFB::KeyCode_Key_Backslash;
        case SDLK_RIGHTBRACKET:
            return ImZeroFB::KeyCode_Key_RightBracket;
        case SDLK_GRAVE:
            return ImZeroFB::KeyCode_Key_GraveAccent;
        case SDLK_CAPSLOCK:
            return ImZeroFB::KeyCode_Key_CapsLock;
        case SDLK_SCROLLLOCK:
            return ImZeroFB::KeyCode_Key_ScrollLock;
        case SDLK_NUMLOCKCLEAR:
            return ImZeroFB::KeyCode_Key_NumLock;
        case SDLK_PRINTSCREEN:
            return ImZeroFB::KeyCode_Key_PrintScreen;
        case SDLK_PAUSE:
            return ImZeroFB::KeyCode_Key_Pause;
        case SDLK_KP_0:
            return ImZeroFB::KeyCode_Key_Keypad0;
        case SDLK_KP_1:
            return ImZeroFB::KeyCode_Key_Keypad1;
        case SDLK_KP_2:
            return ImZeroFB::KeyCode_Key_Keypad2;
        case SDLK_KP_3:
            return ImZeroFB::KeyCode_Key_Keypad3;
        case SDLK_KP_4:
            return ImZeroFB::KeyCode_Key_Keypad4;
        case SDLK_KP_5:
            return ImZeroFB::KeyCode_Key_Keypad5;
        case SDLK_KP_6:
            return ImZeroFB::KeyCode_Key_Keypad6;
        case SDLK_KP_7:
            return ImZeroFB::KeyCode_Key_Keypad7;
        case SDLK_KP_8:
            return ImZeroFB::KeyCode_Key_Keypad8;
        case SDLK_KP_9:
            return ImZeroFB::KeyCode_Key_Keypad9;
        case SDLK_KP_PERIOD:
            return ImZeroFB::KeyCode_Key_KeypadDecimal;
        case SDLK_KP_DIVIDE:
            return ImZeroFB::KeyCode_Key_KeypadDivide;
        case SDLK_KP_MULTIPLY:
            return ImZeroFB::KeyCode_Key_KeypadMultiply;
        case SDLK_KP_MINUS:
            return ImZeroFB::KeyCode_Key_KeypadSubtract;
        case SDLK_KP_PLUS:
            return ImZeroFB::KeyCode_Key_KeypadAdd;
        case SDLK_KP_ENTER:
            return ImZeroFB::KeyCode_Key_KeypadEnter;
        case SDLK_KP_EQUALS:
            return ImZeroFB::KeyCode_Key_KeypadEqual;
        case SDLK_LCTRL:
            return ImZeroFB::KeyCode_Key_LeftCtrl;
        case SDLK_LSHIFT:
            return ImZeroFB::KeyCode_Key_LeftShift;
        case SDLK_LALT:
            return ImZeroFB::KeyCode_Key_LeftAlt;
        case SDLK_LGUI:
            return ImZeroFB::KeyCode_Key_LeftSuper;
        case SDLK_RCTRL:
            return ImZeroFB::KeyCode_Key_RightCtrl;
        case SDLK_RSHIFT:
            return ImZeroFB::KeyCode_Key_RightShift;
        case SDLK_RALT:
            return ImZeroFB::KeyCode_Key_RightAlt;
        case SDLK_RGUI:
            return ImZeroFB::KeyCode_Key_RightSuper;
        case SDLK_APPLICATION:
            return ImZeroFB::KeyCode_Key_Menu;
        case SDLK_0:
            return ImZeroFB::KeyCode_Key_0;
        case SDLK_1:
            return ImZeroFB::KeyCode_Key_1;
        case SDLK_2:
            return ImZeroFB::KeyCode_Key_2;
        case SDLK_3:
            return ImZeroFB::KeyCode_Key_3;
        case SDLK_4:
            return ImZeroFB::KeyCode_Key_4;
        case SDLK_5:
            return ImZeroFB::KeyCode_Key_5;
        case SDLK_6:
            return ImZeroFB::KeyCode_Key_6;
        case SDLK_7:
            return ImZeroFB::KeyCode_Key_7;
        case SDLK_8:
            return ImZeroFB::KeyCode_Key_8;
        case SDLK_9:
            return ImZeroFB::KeyCode_Key_9;
        case SDLK_A:
            return ImZeroFB::KeyCode_Key_A;
        case SDLK_B:
            return ImZeroFB::KeyCode_Key_B;
        case SDLK_C:
            return ImZeroFB::KeyCode_Key_C;
        case SDLK_D:
            return ImZeroFB::KeyCode_Key_D;
        case SDLK_E:
            return ImZeroFB::KeyCode_Key_E;
        case SDLK_F:
            return ImZeroFB::KeyCode_Key_F;
        case SDLK_G:
            return ImZeroFB::KeyCode_Key_G;
        case SDLK_H:
            return ImZeroFB::KeyCode_Key_H;
        case SDLK_I:
            return ImZeroFB::KeyCode_Key_I;
        case SDLK_J:
            return ImZeroFB::KeyCode_Key_J;
        case SDLK_K:
            return ImZeroFB::KeyCode_Key_K;
        case SDLK_L:
            return ImZeroFB::KeyCode_Key_L;
        case SDLK_M:
            return ImZeroFB::KeyCode_Key_M;
        case SDLK_N:
            return ImZeroFB::KeyCode_Key_N;
        case SDLK_O:
            return ImZeroFB::KeyCode_Key_O;
        case SDLK_P:
            return ImZeroFB::KeyCode_Key_P;
        case SDLK_Q:
            return ImZeroFB::KeyCode_Key_Q;
        case SDLK_R:
            return ImZeroFB::KeyCode_Key_R;
        case SDLK_S:
            return ImZeroFB::KeyCode_Key_S;
        case SDLK_T:
            return ImZeroFB::KeyCode_Key_T;
        case SDLK_U:
            return ImZeroFB::KeyCode_Key_U;
        case SDLK_V:
            return ImZeroFB::KeyCode_Key_V;
        case SDLK_W:
            return ImZeroFB::KeyCode_Key_W;
        case SDLK_X:
            return ImZeroFB::KeyCode_Key_X;
        case SDLK_Y:
            return ImZeroFB::KeyCode_Key_Y;
        case SDLK_Z:
            return ImZeroFB::KeyCode_Key_Z;
        case SDLK_F1:
            return ImZeroFB::KeyCode_Key_F1;
        case SDLK_F2:
            return ImZeroFB::KeyCode_Key_F2;
        case SDLK_F3:
            return ImZeroFB::KeyCode_Key_F3;
        case SDLK_F4:
            return ImZeroFB::KeyCode_Key_F4;
        case SDLK_F5:
            return ImZeroFB::KeyCode_Key_F5;
        case SDLK_F6:
            return ImZeroFB::KeyCode_Key_F6;
        case SDLK_F7:
            return ImZeroFB::KeyCode_Key_F7;
        case SDLK_F8:
            return ImZeroFB::KeyCode_Key_F8;
        case SDLK_F9:
            return ImZeroFB::KeyCode_Key_F9;
        case SDLK_F10:
            return ImZeroFB::KeyCode_Key_F10;
        case SDLK_F11:
            return ImZeroFB::KeyCode_Key_F11;
        case SDLK_F12:
            return ImZeroFB::KeyCode_Key_F12;
        case SDLK_F13:
            return ImZeroFB::KeyCode_Key_F13;
        case SDLK_F14:
            return ImZeroFB::KeyCode_Key_F14;
        case SDLK_F15:
            return ImZeroFB::KeyCode_Key_F15;
        case SDLK_F16:
            return ImZeroFB::KeyCode_Key_F16;
        case SDLK_F17:
            return ImZeroFB::KeyCode_Key_F17;
        case SDLK_F18:
            return ImZeroFB::KeyCode_Key_F18;
        case SDLK_F19:
            return ImZeroFB::KeyCode_Key_F19;
        case SDLK_F20:
            return ImZeroFB::KeyCode_Key_F20;
        case SDLK_F21:
            return ImZeroFB::KeyCode_Key_F21;
        case SDLK_F22:
            return ImZeroFB::KeyCode_Key_F22;
        case SDLK_F23:
            return ImZeroFB::KeyCode_Key_F23;
        case SDLK_F24:
            return ImZeroFB::KeyCode_Key_F24;
        case SDLK_AC_BACK:
            return ImZeroFB::KeyCode_Key_AppBack;
        case SDLK_AC_FORWARD:
            return ImZeroFB::KeyCode_Key_AppForward;
        default:
            return ImZeroFB::KeyCode_Key_None;
    }
}
ImGuiKey ImZeroClient::imZeroFBKeyCodeToImGuiKey(const ImZeroFB::KeyCode keyCode) {
    switch(keyCode) {
        case ImZeroFB::KeyCode_Key_Tab:
            return ImGuiKey_Tab;
        case ImZeroFB::KeyCode_Key_LeftArrow:
            return ImGuiKey_LeftArrow;
        case ImZeroFB::KeyCode_Key_RightArrow:
            return ImGuiKey_RightArrow;
        case ImZeroFB::KeyCode_Key_UpArrow:
            return ImGuiKey_UpArrow;
        case ImZeroFB::KeyCode_Key_DownArrow:
            return ImGuiKey_DownArrow;
        case ImZeroFB::KeyCode_Key_PageUp:
            return ImGuiKey_PageUp;
        case ImZeroFB::KeyCode_Key_PageDown:
            return ImGuiKey_PageDown;
        case ImZeroFB::KeyCode_Key_Home:
            return ImGuiKey_Home;
        case ImZeroFB::KeyCode_Key_End:
            return ImGuiKey_End;
        case ImZeroFB::KeyCode_Key_Insert:
            return ImGuiKey_Insert;
        case ImZeroFB::KeyCode_Key_Delete:
            return ImGuiKey_Delete;
        case ImZeroFB::KeyCode_Key_Backspace:
            return ImGuiKey_Backspace;
        case ImZeroFB::KeyCode_Key_Space:
            return ImGuiKey_Space;
        case ImZeroFB::KeyCode_Key_Enter:
            return ImGuiKey_Enter;
        case ImZeroFB::KeyCode_Key_Escape:
            return ImGuiKey_Escape;
        case ImZeroFB::KeyCode_Key_Apostrophe:
            return ImGuiKey_Apostrophe;
        case ImZeroFB::KeyCode_Key_Comma:
            return ImGuiKey_Comma;
        case ImZeroFB::KeyCode_Key_Minus:
            return ImGuiKey_Minus;
        case ImZeroFB::KeyCode_Key_Period:
            return ImGuiKey_Period;
        case ImZeroFB::KeyCode_Key_Slash:
            return ImGuiKey_Slash;
        case ImZeroFB::KeyCode_Key_Semicolon:
            return ImGuiKey_Semicolon;
        case ImZeroFB::KeyCode_Key_Equal:
            return ImGuiKey_Equal;
        case ImZeroFB::KeyCode_Key_LeftBracket:
            return ImGuiKey_LeftBracket;
        case ImZeroFB::KeyCode_Key_Backslash:
            return ImGuiKey_Backslash;
        case ImZeroFB::KeyCode_Key_RightBracket:
            return ImGuiKey_RightBracket;
        case ImZeroFB::KeyCode_Key_GraveAccent:
            return ImGuiKey_GraveAccent;
        case ImZeroFB::KeyCode_Key_CapsLock:
            return ImGuiKey_CapsLock;
        case ImZeroFB::KeyCode_Key_ScrollLock:
            return ImGuiKey_ScrollLock;
        case ImZeroFB::KeyCode_Key_NumLock:
            return ImGuiKey_NumLock;
        case ImZeroFB::KeyCode_Key_PrintScreen:
            return ImGuiKey_PrintScreen;
        case ImZeroFB::KeyCode_Key_Pause:
            return ImGuiKey_Pause;
        case ImZeroFB::KeyCode_Key_Keypad0:
            return ImGuiKey_Keypad0;
        case ImZeroFB::KeyCode_Key_Keypad1:
            return ImGuiKey_Keypad1;
        case ImZeroFB::KeyCode_Key_Keypad2:
            return ImGuiKey_Keypad2;
        case ImZeroFB::KeyCode_Key_Keypad3:
            return ImGuiKey_Keypad3;
        case ImZeroFB::KeyCode_Key_Keypad4:
            return ImGuiKey_Keypad4;
        case ImZeroFB::KeyCode_Key_Keypad5:
            return ImGuiKey_Keypad5;
        case ImZeroFB::KeyCode_Key_Keypad6:
            return ImGuiKey_Keypad6;
        case ImZeroFB::KeyCode_Key_Keypad7:
            return ImGuiKey_Keypad7;
        case ImZeroFB::KeyCode_Key_Keypad8:
            return ImGuiKey_Keypad8;
        case ImZeroFB::KeyCode_Key_Keypad9:
            return ImGuiKey_Keypad9;
        case ImZeroFB::KeyCode_Key_KeypadDecimal:
            return ImGuiKey_KeypadDecimal;
        case ImZeroFB::KeyCode_Key_KeypadDivide:
            return ImGuiKey_KeypadDivide;
        case ImZeroFB::KeyCode_Key_KeypadMultiply:
            return ImGuiKey_KeypadMultiply;
        case ImZeroFB::KeyCode_Key_KeypadSubtract:
            return ImGuiKey_KeypadSubtract;
        case ImZeroFB::KeyCode_Key_KeypadAdd:
            return ImGuiKey_KeypadAdd;
        case ImZeroFB::KeyCode_Key_KeypadEnter:
            return ImGuiKey_KeypadEnter;
        case ImZeroFB::KeyCode_Key_KeypadEqual:
            return ImGuiKey_KeypadEqual;
        case ImZeroFB::KeyCode_Key_LeftCtrl:
            return ImGuiKey_LeftCtrl;
        case ImZeroFB::KeyCode_Key_LeftShift:
            return ImGuiKey_LeftShift;
        case ImZeroFB::KeyCode_Key_LeftAlt:
            return ImGuiKey_LeftAlt;
        case ImZeroFB::KeyCode_Key_LeftSuper:
            return ImGuiKey_LeftSuper;
        case ImZeroFB::KeyCode_Key_RightCtrl:
            return ImGuiKey_RightCtrl;
        case ImZeroFB::KeyCode_Key_RightShift:
            return ImGuiKey_RightShift;
        case ImZeroFB::KeyCode_Key_RightAlt:
            return ImGuiKey_RightAlt;
        case ImZeroFB::KeyCode_Key_RightSuper:
            return ImGuiKey_RightSuper;
        case ImZeroFB::KeyCode_Key_Menu:
            return ImGuiKey_Menu;
        case ImZeroFB::KeyCode_Key_0:
            return ImGuiKey_0;
        case ImZeroFB::KeyCode_Key_1:
            return ImGuiKey_1;
        case ImZeroFB::KeyCode_Key_2:
            return ImGuiKey_2;
        case ImZeroFB::KeyCode_Key_3:
            return ImGuiKey_3;
        case ImZeroFB::KeyCode_Key_4:
            return ImGuiKey_4;
        case ImZeroFB::KeyCode_Key_5:
            return ImGuiKey_5;
        case ImZeroFB::KeyCode_Key_6:
            return ImGuiKey_6;
        case ImZeroFB::KeyCode_Key_7:
            return ImGuiKey_7;
        case ImZeroFB::KeyCode_Key_8:
            return ImGuiKey_8;
        case ImZeroFB::KeyCode_Key_9:
            return ImGuiKey_9;
        case ImZeroFB::KeyCode_Key_A:
            return ImGuiKey_A;
        case ImZeroFB::KeyCode_Key_B:
            return ImGuiKey_B;
        case ImZeroFB::KeyCode_Key_C:
            return ImGuiKey_C;
        case ImZeroFB::KeyCode_Key_D:
            return ImGuiKey_D;
        case ImZeroFB::KeyCode_Key_E:
            return ImGuiKey_E;
        case ImZeroFB::KeyCode_Key_F:
            return ImGuiKey_F;
        case ImZeroFB::KeyCode_Key_G:
            return ImGuiKey_G;
        case ImZeroFB::KeyCode_Key_H:
            return ImGuiKey_H;
        case ImZeroFB::KeyCode_Key_I:
            return ImGuiKey_I;
        case ImZeroFB::KeyCode_Key_J:
            return ImGuiKey_J;
        case ImZeroFB::KeyCode_Key_K:
            return ImGuiKey_K;
        case ImZeroFB::KeyCode_Key_L:
            return ImGuiKey_L;
        case ImZeroFB::KeyCode_Key_M:
            return ImGuiKey_M;
        case ImZeroFB::KeyCode_Key_N:
            return ImGuiKey_N;
        case ImZeroFB::KeyCode_Key_O:
            return ImGuiKey_O;
        case ImZeroFB::KeyCode_Key_P:
            return ImGuiKey_P;
        case ImZeroFB::KeyCode_Key_Q:
            return ImGuiKey_Q;
        case ImZeroFB::KeyCode_Key_R:
            return ImGuiKey_R;
        case ImZeroFB::KeyCode_Key_S:
            return ImGuiKey_S;
        case ImZeroFB::KeyCode_Key_T:
            return ImGuiKey_T;
        case ImZeroFB::KeyCode_Key_U:
            return ImGuiKey_U;
        case ImZeroFB::KeyCode_Key_V:
            return ImGuiKey_V;
        case ImZeroFB::KeyCode_Key_W:
            return ImGuiKey_W;
        case ImZeroFB::KeyCode_Key_X:
            return ImGuiKey_X;
        case ImZeroFB::KeyCode_Key_Y:
            return ImGuiKey_Y;
        case ImZeroFB::KeyCode_Key_Z:
            return ImGuiKey_Z;
        case ImZeroFB::KeyCode_Key_F1:
            return ImGuiKey_F1;
        case ImZeroFB::KeyCode_Key_F2:
            return ImGuiKey_F2;
        case ImZeroFB::KeyCode_Key_F3:
            return ImGuiKey_F3;
        case ImZeroFB::KeyCode_Key_F4:
            return ImGuiKey_F4;
        case ImZeroFB::KeyCode_Key_F5:
            return ImGuiKey_F5;
        case ImZeroFB::KeyCode_Key_F6:
            return ImGuiKey_F6;
        case ImZeroFB::KeyCode_Key_F7:
            return ImGuiKey_F7;
        case ImZeroFB::KeyCode_Key_F8:
            return ImGuiKey_F8;
        case ImZeroFB::KeyCode_Key_F9:
            return ImGuiKey_F9;
        case ImZeroFB::KeyCode_Key_F10:
            return ImGuiKey_F10;
        case ImZeroFB::KeyCode_Key_F11:
            return ImGuiKey_F11;
        case ImZeroFB::KeyCode_Key_F12:
            return ImGuiKey_F12;
        case ImZeroFB::KeyCode_Key_F13:
            return ImGuiKey_F13;
        case ImZeroFB::KeyCode_Key_F14:
            return ImGuiKey_F14;
        case ImZeroFB::KeyCode_Key_F15:
            return ImGuiKey_F15;
        case ImZeroFB::KeyCode_Key_F16:
            return ImGuiKey_F16;
        case ImZeroFB::KeyCode_Key_F17:
            return ImGuiKey_F17;
        case ImZeroFB::KeyCode_Key_F18:
            return ImGuiKey_F18;
        case ImZeroFB::KeyCode_Key_F19:
            return ImGuiKey_F19;
        case ImZeroFB::KeyCode_Key_F20:
            return ImGuiKey_F20;
        case ImZeroFB::KeyCode_Key_F21:
            return ImGuiKey_F21;
        case ImZeroFB::KeyCode_Key_F22:
            return ImGuiKey_F22;
        case ImZeroFB::KeyCode_Key_F23:
            return ImGuiKey_F23;
        case ImZeroFB::KeyCode_Key_F24:
            return ImGuiKey_F24;
        case ImZeroFB::KeyCode_Key_AppBack:
            return ImGuiKey_AppBack;
        case ImZeroFB::KeyCode_Key_AppForward:
            return ImGuiKey_AppForward;
        case ImZeroFB::KeyCode_Key_None: // fallthrough
        default:
            return ImGuiKey_None;
    }
}
#include "imconfig.h"
static void printColorMode(FILE *fd) {
    if (fd == nullptr) { return; }
    fprintf(fd,"IM_COL32_R_SHIFT=%d\n", IM_COL32_R_SHIFT);
    fprintf(fd,"IM_COL32_G_SHIFT=%d\n", IM_COL32_G_SHIFT);
    fprintf(fd,"IM_COL32_B_SHIFT=%d\n", IM_COL32_B_SHIFT);
    fprintf(fd,"IM_COL32_A_SHIFT=%d\n", IM_COL32_A_SHIFT);
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
    fprintf(stderr,"IMGUI_USE_BGRA_PACKED_COLOR set\n");
#else
    fprintf(fd,"IMGUI_USE_BGRA_PACKED_COLOR not set\n");
#endif
}

int ImZeroClient::App::Run(ImZeroCliOptions &opts) {
    printColorMode(stderr);
    // prevent SIGPIPE when writing frames or reading user interaction events
    //signal(SIGPIPE, SIG_IGN);

    sk_sp<SkTypeface> typeface = nullptr;
    sk_sp<SkData> ttfData = nullptr;
    FILE *fffiInFile = stdin;
    FILE *fffiOutFile = stdout;
    {
        // setup skia/imgui shared objects
        if (opts.fFffiInterpreter) {
            if (opts.fFffiInFile != nullptr) {
                fffiInFile = fopen(opts.fFffiInFile, "rw");
                if (fffiInFile == nullptr) {
                    fprintf(stderr, "unable to open fffInFile %s: %s", opts.fFffiInFile, strerror(errno));
                    exit(1);
                }
                setvbuf(fffiInFile, nullptr, _IONBF, 0);
            }
            if (opts.fFffiOutFile != nullptr) {
                fffiOutFile = fopen(opts.fFffiOutFile, "w");
                if (fffiOutFile == nullptr) {
                    fprintf(stderr, "unable to open fffOutFile %s: %s", opts.fFffiOutFile, strerror(errno));
                    exit(1);
                }
                setvbuf(fffiOutFile, nullptr, _IONBF, 0);
            }
        }
    }

    if (opts.fFffiInterpreter) {
        render_init(fffiInFile, fffiOutFile);
    }
    return 0;
}
int ImZeroClient::App::mainLoopHeadless(const ImZeroCliOptions &opts, ImVec4 const &clearColor) {
    ImGuiIO &io = ImGui::GetIO();
    fOutputFormat = resolveRawFrameOutputFormat(opts.fVideoRawOutputFormat);
    auto const w = static_cast<float>(opts.fVideoResolutionWidth);
    auto const h = static_cast<float>(opts.fVideoResolutionHeight);
    io.DisplaySize.x = w;
    io.DisplaySize.y = h;

    if(opts.fVideoUserInteractionEventsFile != nullptr && opts.fVideoUserInteractionEventsFile[0] != '\0') {
        // RDWR: having at least one writer will prevent SIG_PIPE
        fUserInteractionFd = open(opts.fVideoUserInteractionEventsFile, O_RDWR | O_NONBLOCK);
        if(fUserInteractionFd == -1) {
            fprintf(stderr, "unable to open user interaction events in file %s: %s\n", opts.fVideoUserInteractionEventsFile, strerror(errno));
            return 1;
        }
        fInteractionEventsAreInBinary = opts.fVideoUserInteractionEventsAreBinary;
        fInteractionFBBuilder = flatbuffers::FlatBufferBuilder();
        fDispatchInteractionEvents = true;
    }
    fRawFrameFileOpened = false;

    switch(fOutputFormat) {
        case kRawFrameOutputFormat_None:
            loopEmpty(opts);
            break;
        case kRawFrameOutputFormat_WebP_Lossless: // fallthrough
        case kRawFrameOutputFormat_WebP_Lossy:
            loopWebp(opts);
            break;
        case kRawFrameOutputFormat_Jpeg:
            loopJpeg(opts);
            break;
        case kRawFrameOutputFormat_Png:
            loopPng(opts);
            break;
        case kRawFrameOutputFormat_Qoi:
            loopQoi(opts);
            break;
        case kRawFrameOutputFormat_Bmp_Bgra8888:
            loopBmp(opts);
            break;
        case kRawFrameOutputFormat_Flatbuffers:
            loopFlatbuffers(opts);
            break;
        case kRawFrameOutputFormat_Svg: // fallthrough
        case kRawFrameOutputFormat_Svg_TextAsPath:
            loopSvg(opts);
            break;
        case kRawFrameOutputFormat_Skp:
            loopSkp(opts);
            break;
    }
    return 0;
}
void ImZeroClient::App::loopEmpty(const ImZeroCliOptions &opts) {
    const int w = static_cast<int>(opts.fVideoResolutionWidth);
    const int h = static_cast<int>(opts.fVideoResolutionHeight);
    const uint64_t maxFrame = opts.fVideoExitAfterNFrames;

    while(maxFrame == 0 || fFrame < maxFrame) {
        videoPaint(nullptr,w,h);
        videoPostPaint();
    }
}

void ImZeroClient::App::loopWebp(const ImZeroCliOptions &opts) {
    const int w = static_cast<int>(opts.fVideoResolutionWidth);
    const int h = static_cast<int>(opts.fVideoResolutionHeight);
    SkWebpEncoder::Options webPOptions;

    auto const rasterSurface = getSurfaceRaster(w,h);
    const auto canvas = rasterSurface->getCanvas();

    if(fOutputFormat == kRawFrameOutputFormat_WebP_Lossy) {
        webPOptions.fCompression = SkWebpEncoder::Compression::kLossy;
        webPOptions.fQuality = 70.0f;
    } else {
        webPOptions.fCompression = SkWebpEncoder::Compression::kLossless;
        webPOptions.fQuality = 0.0f;
    }
    const uint64_t maxFrame = opts.fVideoExitAfterNFrames;

    videoPaint(canvas,w,h);
    videoPostPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        videoPaint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if(img->peekPixels(&pixmap)) {
                ensureRawFrameFileOpened(opts);
                if (!SkWebpEncoder::Encode(static_cast<SkWStream *>(fRawFrameFileStream.get()), pixmap, webPOptions)) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                    fRawFrameFileOpened = false;
                }
            }
        }
        videoPostPaint();
    }
}

void ImZeroClient::App::loopJpeg(const ImZeroCliOptions &opts) {
    const int w = static_cast<int>(opts.fVideoResolutionWidth);
    const int h = static_cast<int>(opts.fVideoResolutionHeight);
    SkJpegEncoder::Options jpegOptions;

    auto const rasterSurface = getSurfaceRaster(w,h);
    const auto canvas = rasterSurface->getCanvas();

    jpegOptions.fQuality = 80;
    const uint64_t maxFrame = opts.fVideoExitAfterNFrames;

    videoPaint(canvas,w,h);
    videoPostPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        videoPaint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if(img->peekPixels(&pixmap)) {
                ensureRawFrameFileOpened(opts);
                if (!SkJpegEncoder::Encode(static_cast<SkWStream *>(fRawFrameFileStream.get()), pixmap, jpegOptions)) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                    fRawFrameFileOpened = false;
                }
            }
        }
        videoPostPaint();
    }
}
void ImZeroClient::App::loopPng(const ImZeroCliOptions &opts) {
    const int w = static_cast<int>(opts.fVideoResolutionWidth);
    const int h = static_cast<int>(opts.fVideoResolutionHeight);
    SkPngEncoder::Options pngOptions;

    auto const rasterSurface = getSurfaceRaster(w,h);
    const auto canvas = rasterSurface->getCanvas();

    pngOptions.fZLibLevel = 6;
    const uint64_t maxFrame = opts.fVideoExitAfterNFrames;

    videoPaint(canvas,w,h);
    videoPostPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        videoPaint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if(img->peekPixels(&pixmap)) {
                ensureRawFrameFileOpened(opts);
                if (!SkPngEncoder::Encode(static_cast<SkWStream *>(fRawFrameFileStream.get()), pixmap, pngOptions)) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                    fRawFrameFileOpened = false;
                }
            }
        }
        videoPostPaint();
    }
}
void ImZeroClient::App::loopQoi(const ImZeroCliOptions &opts) {
    const int w = static_cast<int>(opts.fVideoResolutionWidth);
    const int h = static_cast<int>(opts.fVideoResolutionHeight);
    qoi_desc qd{
            static_cast<unsigned int>(w),
            static_cast<unsigned int>(h),
            4,
            QOI_SRGB
    };

    auto const rasterSurface = getSurfaceRaster(w,h);
    const auto canvas = rasterSurface->getCanvas();
    auto stream = SkFILEWStream(opts.fVideoRawFramesFile);
    const uint64_t maxFrame = opts.fVideoExitAfterNFrames;

    videoPaint(canvas,w,h);
    videoPostPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        videoPaint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if (img->peekPixels(&pixmap)) {
                int imgLen;
                auto const imgMem = qoi_encode(pixmap.addr(), &qd, &imgLen);
                stream.write(imgMem, static_cast<size_t>(imgLen));
                stream.flush();
            }
        }
        videoPostPaint();
    }
}
void ImZeroClient::App::loopBmp(const ImZeroCliOptions &opts) {
    const int w = static_cast<int>(opts.fVideoResolutionWidth);
    const int h = static_cast<int>(opts.fVideoResolutionHeight);
    const auto bmpEncoder = BmpBGRA8888Encoder(w,h);

    auto const rasterSurface = getSurfaceRaster(w,h);
    const auto canvas = rasterSurface->getCanvas();

    const uint64_t maxFrame = opts.fVideoExitAfterNFrames;

    videoPaint(canvas,w,h);
    videoPostPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        videoPaint(canvas,w,h);

        { ZoneScoped;
            SkPixmap pixmap;
            sk_sp<SkImage> img(rasterSurface->makeImageSnapshot(SkIRect::MakeWH(w,h)));
            if(img->peekPixels(&pixmap)) {
                ensureRawFrameFileOpened(opts);
                if (!bmpEncoder.encode(static_cast<SkWStream *>(fRawFrameFileStream.get()), pixmap.addr32())) {
                    fprintf(stderr,"unable to encode frame as image. Skipping.\n");
                    fRawFrameFileOpened = false;
                }
            }
        }
        videoPostPaint();
    }
}
void ImZeroClient::App::loopFlatbuffers(const ImZeroCliOptions &opts) {
    const int w = static_cast<int>(opts.fVideoResolutionWidth);
    const int h = static_cast<int>(opts.fVideoResolutionHeight);
    auto stream = SkFILEWStream(opts.fVideoRawFramesFile);
    const uint64_t maxFrame = opts.fVideoExitAfterNFrames;
    fFrame = 0;
    fApp.fVectorCmdSkiaRenderer.changeRenderMode(ImGuiSkia::RenderModeE_Normal);
    fApp.fUseVectorCmd = true;
    videoPaint(nullptr,w,h);
    videoPostPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        fApp.fUseVectorCmd = true;
        videoPaint(nullptr,w,h);
        { ZoneScoped;
            const ImDrawData* drawData = ImGui::GetDrawData();
            for (int i = 0; i < drawData->CmdListsCount; ++i) {
                ImDrawList* drawList = drawData->CmdLists[i];
                const uint8_t *buf;
                size_t sz;
                drawList->serializeFB(buf,sz);
                stream.write(buf,sz);
            }
            stream.flush();
        }
        videoPostPaint();
    }
}
void ImZeroClient::App::loopSvg(const ImZeroCliOptions &opts) {
    const int w = static_cast<int>(opts.fVideoResolutionWidth);
    const int h = static_cast<int>(opts.fVideoResolutionHeight);
    int svgCanvasFlags = 0;

    auto stream = SkFILEWStream(opts.fVideoRawFramesFile);
    SkCanvas *canvas = nullptr;

    switch(fOutputFormat) {
        case kRawFrameOutputFormat_Svg:
            svgCanvasFlags = SkSVGCanvas::kNoPrettyXML_Flag | SkSVGCanvas::kRelativePathEncoding_Flag;
            fApp.fVectorCmdSkiaRenderer.changeRenderMode(fApp.fVectorCmdSkiaRenderer.getRenderMode() | ImGuiSkia::RenderModeE_SVG);
            canvas = SkSVGCanvas::Make(SkRect::MakeIWH(w,h), &stream, svgCanvasFlags).release();
            break;
        case kRawFrameOutputFormat_Svg_TextAsPath:
            svgCanvasFlags = SkSVGCanvas::kNoPrettyXML_Flag | SkSVGCanvas::kRelativePathEncoding_Flag | SkSVGCanvas::kConvertTextToPaths_Flag;
            fApp.fVectorCmdSkiaRenderer.changeRenderMode(fApp.fVectorCmdSkiaRenderer.getRenderMode() | ImGuiSkia::RenderModeE_SVG);
            canvas = SkSVGCanvas::Make(SkRect::MakeIWH(w,h), &stream, svgCanvasFlags).release();
            break;
    }
    const uint64_t maxFrame = opts.fVideoExitAfterNFrames;

    videoPaint(canvas,w,h);
    videoPostPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        videoPaint(canvas,w,h);
        videoPostPaint();
    }

    // delete SkSvgCanvas to flush commands (no necessary for other canvas implementations)
    delete canvas;
}
void ImZeroClient::App::loopSkp(const ImZeroCliOptions &opts) {
    const int w = static_cast<int>(opts.fVideoResolutionWidth);
    const int h = static_cast<int>(opts.fVideoResolutionHeight);

    SkPictureRecorder skiaRecorder;
    auto stream = SkFILEWStream(opts.fVideoRawFramesFile);

    const uint64_t maxFrame = opts.fVideoExitAfterNFrames;

    videoPaint(nullptr,w,h);
    videoPostPaint();
    fFrame = 0;
    while(maxFrame == 0 || fFrame < maxFrame) {
        const auto canvas = skiaRecorder.beginRecording(SkIntToScalar(w), SkIntToScalar(h));
        videoPaint(canvas,w,h);

        { ZoneScoped;
            sk_sp<SkPicture> picture = skiaRecorder.finishRecordingAsPicture();
            picture->serialize(&stream);
        }
        videoPostPaint();
    }
}
void ImZeroClient::App::videoPostPaint() {
    ImGuiIO& io = ImGui::GetIO();

    static uint64_t frequency = SDL_GetPerformanceFrequency();
    uint64_t current_time = SDL_GetPerformanceCounter();
    if(current_time <= fTime) {
        current_time = fTime + 1;
    }
    io.DeltaTime = fTime > 0 ? (float)((double)(current_time - fTime) / static_cast<double>(frequency)) : (float)(1.0f / 60.0f);
    fTime = current_time;

    fFrame++;
    if(fInteractionEventsAreInBinary) {
        dispatchUserInteractionEventsBinary();
    } else {
        dispatchUserInteractionEventsFB();
    }
}
void ImZeroClient::App::videoPaint(SkCanvas* canvas, int width, int height) { ZoneScoped;
    ImGui::useVectorCmd = fApp.fUseVectorCmd;
    resetReceiveStat();
    ::resetSendStat();
    if(fFffiInterpreter) { ZoneScopedN("render fffi commands");
        render_render();
    } else { ZoneScopedN("demo window");
        ImGui::ShowDemoWindow();
    }
/*
    auto const saveFormat = ImGuiSkia::FrameExportFormatE_Disabled;
    if(ImGui::Begin("ImZeroSkia Settings")) { ZoneScoped;
        fImZeroSkiaSetupUi.render(saveFormat, fVectorCmdSkiaRenderer, fUseVectorCmd,
                                  fTotalVectorCmdSerializedSize, totalSentBytes+totalReceivedBytes,
                                  fSkpBytesWritten, fSvgBytesWritten, fPngBytesWritten,
                                  width, height
        );
    }

    if(canvas != nullptr) { ZoneScoped;
        canvas->clear(fBackgroundColor);

        canvas->save();
        drawImGuiVectorCmdsFB(*canvas);
        canvas->restore();
    }
    */
}
ImZeroClient::App::~App() {
    if(fDispatchInteractionEvents) {
        close(fUserInteractionFd);
        fDispatchInteractionEvents = false;
    }
}
template <typename T>
static T binaryUnmarshallIntegral(uint8_t **mem) {
    T v;
    memcpy(&v,*mem,sizeof(T));
    (*mem) += sizeof(T);
    return v;
}
static const char *binaryUnmarshallString(uint8_t **mem,uint32_t &lenExclTermination) {
    memcpy(&lenExclTermination,*mem,sizeof(lenExclTermination));
    (*mem) += sizeof(lenExclTermination); // +1: zero termination
    const char *r = reinterpret_cast<const char*>(*mem);
    (*mem) += lenExclTermination+1;
    return r;
}
void ImZeroClient::App::dispatchUserInteractionEventsBinary() {
    constexpr uint8_t eventType_MouseMotion = 0;
    constexpr uint8_t eventType_MouseButton = 1;
    constexpr uint8_t eventType_MouseWheel = 2;
    constexpr uint8_t eventType_ClientConnect = 3;
    constexpr uint8_t eventType_ClientDisconnect = 4;
    constexpr uint8_t eventType_InputText = 5;
    constexpr uint8_t eventType_InputKeyboard = 6;

    static size_t memorySize = 1024 * 1024;
    static uint8_t state = 0;
    static uint8_t* mem = nullptr;
    static uint8_t* p = nullptr;
    static uint32_t bytesToRead = 0;
    constexpr const int sizeOfLengthPrefix = 4;

    if(!fDispatchInteractionEvents) {
        return;
    }
    while(true) {
        switch(state) {
            case 0: // init
                mem = static_cast<uint8_t *>(malloc(memorySize));
                if(mem == nullptr) {
                    fprintf(stderr,"unable to allocate memory\n");
                    exit(2);
                }
                state = 1;
                bytesToRead = sizeOfLengthPrefix;
                p = mem;
                break;
            case 1: // read message length
            {
                auto r = read(fUserInteractionFd,p,bytesToRead);
                if(r <= 0) {
                    return;
                }
                bytesToRead -= r;
                p += r;
                if(bytesToRead == 0) {
                    // read length of message
                    memcpy(&bytesToRead,mem,sizeof(bytesToRead));
                    if(bytesToRead > memorySize) {
                        memorySize = (bytesToRead/4096+1)*4096;
                        mem = static_cast<uint8_t *>(realloc(mem, memorySize));
                        p = mem+sizeOfLengthPrefix;
                    }
                    state = 2;
                }
            }
                break;
            case 2: // read message
            {
                auto r = read(fUserInteractionFd,p,bytesToRead);
                bytesToRead -= r;
                p += r;
                if(bytesToRead == 0) {
                    flatbuffers::Offset ev = 0;
                    ImZeroFB::UserInteraction t = ImZeroFB::UserInteraction_NONE;
                    p = mem+sizeOfLengthPrefix;
                    auto const et = binaryUnmarshallIntegral<uint8_t>(&p);
                    switch(et) {
                        case eventType_MouseMotion:
                        {
                            auto const x = binaryUnmarshallIntegral<float>(&p);
                            auto const y = binaryUnmarshallIntegral<float>(&p);
                            auto const mouseId = binaryUnmarshallIntegral<uint32_t>(&p);
                            auto const isTouch = binaryUnmarshallIntegral<uint8_t >(&p) != 0;
                            auto const posFB = ImZeroFB::SingleVec2(x,y);
                            ev = ImZeroFB::CreateEventMouseMotion(fInteractionFBBuilder,&posFB,mouseId,isTouch).Union();
                            t = ImZeroFB::UserInteraction_EventMouseMotion;
                        }
                            break;
                        case eventType_MouseButton:
                        {
                            auto const x = binaryUnmarshallIntegral<float>(&p);
                            auto const y = binaryUnmarshallIntegral<float>(&p);
                            auto const mouseId = binaryUnmarshallIntegral<uint32_t>(&p);
                            auto const isTouch = binaryUnmarshallIntegral<uint8_t >(&p) != 0;
                            auto const buttons = binaryUnmarshallIntegral<uint8_t >(&p);
                            auto const isDown = binaryUnmarshallIntegral<uint8_t >(&p) != 0;

                            auto const posFB = ImZeroFB::SingleVec2(x,y);
                            auto buttonsFB = ImZeroFB::MouseButtons_NONE;
                            buttonsFB = static_cast<ImZeroFB::MouseButtons>(buttonsFB | (((buttons & 0b1) != 0) ? ImZeroFB::MouseButtons_Left : ImZeroFB::MouseButtons_NONE));
                            buttonsFB = static_cast<ImZeroFB::MouseButtons>(buttonsFB | (((buttons & 0b10) != 0) ? ImZeroFB::MouseButtons_Middle : ImZeroFB::MouseButtons_NONE));
                            buttonsFB = static_cast<ImZeroFB::MouseButtons>(buttonsFB | (((buttons & 0b100) != 0) ? ImZeroFB::MouseButtons_Right : ImZeroFB::MouseButtons_NONE));
                            buttonsFB = static_cast<ImZeroFB::MouseButtons>(buttonsFB | (((buttons & 0b1000) != 0) ? ImZeroFB::MouseButtons_X1 : ImZeroFB::MouseButtons_NONE));
                            buttonsFB = static_cast<ImZeroFB::MouseButtons>(buttonsFB | (((buttons & 0b10000) != 0) ? ImZeroFB::MouseButtons_X2 : ImZeroFB::MouseButtons_NONE));
                            auto const etFB = isDown ? ImZeroFB::MouseButtonEventType_Down : ImZeroFB::MouseButtonEventType_Up;
                            ev = ImZeroFB::CreateEventMouseButton(fInteractionFBBuilder,&posFB,mouseId,isTouch,buttonsFB,etFB).Union();
                            t = ImZeroFB::UserInteraction_EventMouseButton;
                        }
                            break;
                        case eventType_MouseWheel:
                        {
                            auto const x = binaryUnmarshallIntegral<float>(&p);
                            auto const y = binaryUnmarshallIntegral<float>(&p);
                            auto const mouseId = binaryUnmarshallIntegral<uint32_t>(&p);
                            auto const isTouch = binaryUnmarshallIntegral<uint8_t >(&p) != 0;

                            auto const posFB = ImZeroFB::SingleVec2(x,y);
                            ev = ImZeroFB::CreateEventMouseWheel(fInteractionFBBuilder,&posFB,mouseId,isTouch).Union();
                            t = ImZeroFB::UserInteraction_EventMouseWheel;
                        }
                            break;
                        case eventType_InputKeyboard:
                        {
                            uint32_t symNameLen;
                            auto const modifiers = binaryUnmarshallIntegral<uint16_t>(&p);
                            auto const symName = binaryUnmarshallString(&p,symNameLen);
                            auto const isDown = binaryUnmarshallIntegral<uint8_t>(&p) != 0;
                            auto const nativeSym = binaryUnmarshallIntegral<uint32_t>(&p);
                            auto const scanCode = binaryUnmarshallIntegral<uint32_t>(&p);

                            auto modifiersFB = ImZeroFB::KeyModifiers_NONE;
                            modifiersFB = static_cast<ImZeroFB::KeyModifiers>(modifiersFB | (((modifiers & 0b1) != 0) ? ImZeroFB::KeyModifiers_LeftShift : ImZeroFB::KeyModifiers_NONE));
                            modifiersFB = static_cast<ImZeroFB::KeyModifiers>(modifiersFB | (((modifiers & 0b10) != 0) ? ImZeroFB::KeyModifiers_RightShift : ImZeroFB::KeyModifiers_NONE));
                            modifiersFB = static_cast<ImZeroFB::KeyModifiers>(modifiersFB | (((modifiers & 0b100) != 0) ? ImZeroFB::KeyModifiers_LeftCtrl : ImZeroFB::KeyModifiers_NONE));
                            modifiersFB = static_cast<ImZeroFB::KeyModifiers>(modifiersFB | (((modifiers & 0b1000) != 0) ? ImZeroFB::KeyModifiers_RightCtrl : ImZeroFB::KeyModifiers_NONE));
                            modifiersFB = static_cast<ImZeroFB::KeyModifiers>(modifiersFB | (((modifiers & 0b10000) != 0) ? ImZeroFB::KeyModifiers_LeftAlt : ImZeroFB::KeyModifiers_NONE));
                            modifiersFB = static_cast<ImZeroFB::KeyModifiers>(modifiersFB | (((modifiers & 0b100000) != 0) ? ImZeroFB::KeyModifiers_RightAlt : ImZeroFB::KeyModifiers_NONE));
                            modifiersFB = static_cast<ImZeroFB::KeyModifiers>(modifiersFB | (((modifiers & 0b1000000) != 0) ? ImZeroFB::KeyModifiers_LeftSuper : ImZeroFB::KeyModifiers_NONE));
                            modifiersFB = static_cast<ImZeroFB::KeyModifiers>(modifiersFB | (((modifiers & 0b10000000) != 0) ? ImZeroFB::KeyModifiers_RightSuper : ImZeroFB::KeyModifiers_NONE));

                            auto keyCodeFB = ImZeroFB::KeyCode_Key_None;
                            if(symNameLen == 0) {
                                ImGuiIO& io = ImGui::GetIO();
                                SDL_Keymod currentMod = 0;
                                if(io.KeyAlt) {
                                    currentMod |= SDL_KMOD_ALT;
                                }
                                if(io.KeyShift) {
                                    currentMod |= SDL_KMOD_SHIFT;
                                }
                                if(io.KeyCtrl) {
                                    currentMod |= SDL_KMOD_CTRL;
                                }
                                if(io.KeySuper) {
                                    currentMod |= SDL_KMOD_GUI;
                                }
                                if(isDown) {
                                    if((modifiersFB & (ImZeroFB::KeyModifiers_LeftAlt | ImZeroFB::KeyModifiers_RightAlt)) != 0) {
                                        currentMod |= SDL_KMOD_ALT;
                                    }
                                    if((modifiersFB & (ImZeroFB::KeyModifiers_LeftShift | ImZeroFB::KeyModifiers_RightShift)) != 0) {
                                        currentMod |= SDL_KMOD_SHIFT;
                                    }
                                    if((modifiersFB & (ImZeroFB::KeyModifiers_LeftCtrl | ImZeroFB::KeyModifiers_RightCtrl)) != 0) {
                                        currentMod |= SDL_KMOD_CTRL;
                                    }
                                    if((modifiersFB & (ImZeroFB::KeyModifiers_LeftSuper | ImZeroFB::KeyModifiers_RightSuper)) != 0) {
                                        currentMod |= SDL_KMOD_GUI;
                                    }
                                } else {
                                    if((modifiersFB & (ImZeroFB::KeyModifiers_LeftAlt | ImZeroFB::KeyModifiers_RightAlt)) != 0) {
                                        currentMod &= (~SDL_KMOD_ALT);
                                    }
                                    if((modifiersFB & (ImZeroFB::KeyModifiers_LeftShift | ImZeroFB::KeyModifiers_RightShift)) != 0) {
                                        currentMod &= (~SDL_KMOD_SHIFT);
                                    }
                                    if((modifiersFB & (ImZeroFB::KeyModifiers_LeftCtrl | ImZeroFB::KeyModifiers_RightCtrl)) != 0) {
                                        currentMod &= (~SDL_KMOD_CTRL);
                                    }
                                    if((modifiersFB & (ImZeroFB::KeyModifiers_LeftSuper | ImZeroFB::KeyModifiers_RightSuper)) != 0) {
                                        currentMod &= (~SDL_KMOD_GUI);
                                    }
                                }
                                // use server-side keyboard layout encoded in SDL
                                keyCodeFB = sdlKeyCodeToImZeroFBKeyCode(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(scanCode),currentMod,false), static_cast<SDL_Scancode>(scanCode));
                            } else {
                                // FIXME
                            }


                            ev = ImZeroFB::CreateEventKeyboard(fInteractionFBBuilder,modifiersFB,keyCodeFB,isDown,nativeSym,scanCode).Union();
                            t = ImZeroFB::UserInteraction_EventKeyboard;
                        }
                            break;
                        case eventType_InputText:
                        {
                            uint32_t textLen;
                            auto const text = binaryUnmarshallString(&p,textLen);

                            auto const textFB = fInteractionFBBuilder.CreateString(text,static_cast<size_t>(textLen));

                            ev = ImZeroFB::CreateEventTextInput(fInteractionFBBuilder,textFB).Union();
                            t = ImZeroFB::UserInteraction_EventTextInput;
                        }
                            break;
                        case eventType_ClientConnect:
                            fInteractionClientConnected = true;
                            break;
                        case eventType_ClientDisconnect:
                            fInteractionClientConnected = false;
                            break;
                    }
                    if(t != ImZeroFB::UserInteraction_NONE) {
                        fInteractionFBBuilder.Finish(ImZeroFB::CreateInputEvent(fInteractionFBBuilder,t,ev));
                        auto const buf = fInteractionFBBuilder.GetBufferPointer();
                        auto const sz = fInteractionFBBuilder.GetSize();
                        auto const iev = flatbuffers::GetRoot<ImZeroFB::InputEvent>(buf);
                        if(iev == nullptr) {
                            fprintf(stderr,"unable to get ImZeroFB::InputEvent root, skipping.");
                        } else {
                            auto txt = flatbuffers::FlatBufferToString(buf,ImZeroFB::InputEventTypeTable());
                            fprintf(stderr, "userInteractionEvent(%d Bytes)=%s\n", static_cast<int>(sz),txt.c_str());
                            handleUserInteractionEvent(*iev);
                        }
                        fInteractionFBBuilder.Clear();
                    }
                    state = 1;
                    p = mem;
                    bytesToRead = sizeOfLengthPrefix;
                }
            }
                break;
        }
    }
}
void ImZeroClient::App::dispatchUserInteractionEventsFB() {
    static size_t memorySize = 1024 * 1024;
    static uint8_t state = 0;
    static uint8_t* mem = nullptr;
    static uint8_t* p = nullptr;
    static uint32_t bytesToRead = 0;
    constexpr const int sizeOfLengthPrefix = 4;

    if(!fDispatchInteractionEvents) {
        return;
    }
    while(true) {
        switch(state) {
            case 0: // init
                mem = static_cast<uint8_t *>(malloc(memorySize));
                if(mem == nullptr) {
                    fprintf(stderr,"unable to allocate memory\n");
                    exit(2);
                }
                state = 1;
                bytesToRead = sizeOfLengthPrefix;
                p = mem;
                break;
            case 1: // read flatbuffers message length
            {
                auto r = read(fUserInteractionFd,p,bytesToRead);
                if(r <= 0) {
                    return;
                }
                bytesToRead -= r;
                p += r;
                if(bytesToRead == 0) {
                    // read length of message
                    bytesToRead = flatbuffers::ReadScalar<uint32_t>(mem);
                    if(bytesToRead > memorySize) {
                        memorySize = (bytesToRead/4096+1)*4096;
                        mem = static_cast<uint8_t *>(realloc(mem, memorySize));
                        p = mem+sizeOfLengthPrefix;
                    }
                    state = 2;
                }
            }
                break;
            case 2: // read flatbuffers message
            {
                //fprintf(stderr, "reading message of size %d\n", (int)bytesToRead);
                auto r = read(fUserInteractionFd,p,bytesToRead);
                bytesToRead -= r;
                p += r;
                if(bytesToRead == 0) {
                    auto verifier = flatbuffers::Verifier(mem+sizeOfLengthPrefix,bytesToRead);
                    if(!verifier.VerifyBuffer<ImZeroFB::InputEvent>()) {
                        /*auto txt = flatbuffers::FlatBufferToString(mem+sizeOfLengthPrefix,ImZeroFB::InputEventTypeTable());
                        fprintf(stderr, "userInteractionEvent=%s\n", txt.c_str());
                         */

                        auto const e = flatbuffers::GetSizePrefixedRoot<ImZeroFB::InputEvent>(mem);
                        if(e == nullptr) {
                            fprintf(stderr,"unable to get ImZeroFB::InputEvent root, skipping.");
                        } else {
                            handleUserInteractionEvent(*e);
                        }
                        state = 1;
                        p = mem;
                        bytesToRead = sizeOfLengthPrefix;
                    } else {
                        fprintf(stderr, "received corrupted user interaction event!\n");
                        exit(1);
                    }
                }
            }
                break;
        }
    }
}

void ImZeroClient::App::handleUserInteractionEvent(ImZeroFB::InputEvent const &ev) {
    ImGuiIO& io = ImGui::GetIO();
    //fprintf(stderr, "handling user interaction event %s\n",EnumNameUserInteraction(ev.event_type()));
    switch(ev.event_type()) {
        case ImZeroFB::UserInteraction_NONE:
            break;
        case ImZeroFB::UserInteraction_EventMouseMotion:
        {
            auto const e = ev.event_as_EventMouseMotion();
            io.AddMouseSourceEvent(e->is_touch() ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
            auto const p = e->pos();
            io.AddMousePosEvent(p->x(), p->y());
        }
            break;
        case ImZeroFB::UserInteraction_EventMouseWheel:
        {
            auto const e = ev.event_as_EventMouseWheel();
            io.AddMouseSourceEvent(e->is_touch() ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
            auto const p = e->pos();
            io.AddMouseWheelEvent(-p->x(), p->y());
        }
            break;
        case ImZeroFB::UserInteraction_EventMouseButton:
        {
            auto const e = ev.event_as_EventMouseButton();
            int mb = 0;
            bool skip = false;
            auto const b = e->buttons();
            switch(b) {
                case ImZeroFB::MouseButtons_Left:
                    mb = ImGuiMouseButton_Left;
                    break;
                case ImZeroFB::MouseButtons_Right:
                    mb = ImGuiMouseButton_Right;
                    break;
                case ImZeroFB::MouseButtons_Middle:
                    mb = ImGuiMouseButton_Middle;
                    break;
                case ImZeroFB::MouseButtons_X1: // fallthrough
                case ImZeroFB::MouseButtons_X2: // fallthrough
                default:
                    skip = true;
            }
            if(!skip) {
                const bool d = e->type() == ImZeroFB::MouseButtonEventType_Down;
                io.AddMouseSourceEvent(e->is_touch() ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
                io.AddMouseButtonEvent(mb, d);
            }
        }
            break;
        case ImZeroFB::UserInteraction_EventKeyboard:
        {
            auto const e = ev.event_as_EventKeyboard();
            auto const keyMod = e->modifiers();
            auto const key= imZeroFBKeyCodeToImGuiKey(e->code());
            io.AddKeyEvent(ImGuiMod_Ctrl, (keyMod & (ImZeroFB::KeyModifiers_LeftCtrl | ImZeroFB::KeyModifiers_RightCtrl)) != 0);
            io.AddKeyEvent(ImGuiMod_Shift, (keyMod & (ImZeroFB::KeyModifiers_LeftShift | ImZeroFB::KeyModifiers_RightShift)) != 0);
            io.AddKeyEvent(ImGuiMod_Alt, (keyMod & (ImZeroFB::KeyModifiers_LeftAlt | ImZeroFB::KeyModifiers_RightAlt)) != 0);
            io.AddKeyEvent(ImGuiMod_Super, (keyMod & (ImZeroFB::KeyModifiers_LeftSuper | ImZeroFB::KeyModifiers_RightSuper)) != 0);
            io.AddKeyEvent(key, e->is_down());
            //io.SetKeyEventNativeData(key,static_cast<int>(e->native_sym()),static_cast<int>(e->scancode()));
        }
            break;
        case ImZeroFB::UserInteraction_EventTextInput:
        {
            auto const e = ev.event_as_EventTextInput();
            auto const t = e->text();

            io.AddInputCharactersUTF8(t->c_str());
        }
            break;
        case ImZeroFB::UserInteraction_EventClientConnect:
            break;
        case ImZeroFB::UserInteraction_EventClientDisconnect:
            break;
        case ImZeroFB::UserInteraction_EventClientKeepAlive:
            break;
        case ImZeroFB::UserInteraction_EventKeepAlive:
            break;
    }
}

void ImZeroClient::App::ensureRawFrameFileOpened(const ImZeroCliOptions &opts) {
    if(!fRawFrameFileOpened || fRawFrameFileStream == nullptr) {
        if(fRawFrameFileStream != nullptr) {
            fprintf(stderr,"re-opening raw frames file\n");
            fRawFrameFileStream.reset();
        }
        fRawFrameFileStream = std::make_unique<SkFILEWStream>(opts.fVideoRawFramesFile);
        if(!fRawFrameFileStream->isValid()) {
            fprintf(stderr,"unable to open video raw frame file %s\n",opts.fVideoRawFramesFile);
            exit(1);
        }
        fRawFrameFileOpened = true;
    }
}
sk_sp<SkSurface> ImZeroClient::App::getSurfaceRaster(int const w, int const h) {
    if(fSurface == nullptr) {
        constexpr SkColorType colorType = kRGBA_8888_SkColorType;
        constexpr SkAlphaType alphaType = kPremul_SkAlphaType;
        const sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
        const auto c = SkColorInfo(colorType, alphaType, colorSpace);
        fSurface = SkSurfaces::Raster(SkImageInfo::Make(SkISize::Make(w,h), c));
    }
    return fSurface;
}

ImZeroClient::App::App() {
    fFffiInterpreter = false;

    fFrame = 0;
    fPreviousTime = 0.0;
    fOutputFormat = kRawFrameOutputFormat_None;
    fUserInteractionFd = 0;
}