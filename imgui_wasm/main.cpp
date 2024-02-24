#include "imconfig.h"
#include "imgui.h"
#include <stdio.h>
#include "src/marshalling/casts.h"
#include "render.h"

#include "imgui/emscripten_mainloop_stub.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int main(int, char**)
{
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN {
    }
    EMSCRIPTEN_MAINLOOP_END;
    ImGui::DestroyContext();
    return 0;
}
