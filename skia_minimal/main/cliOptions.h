#pragma once
#include <cstdio>
auto constexpr defaultTtfFilePath = "./SauceCodeProNerdFontPropo-Regular.ttf";

struct CliOptions {
    const char *ttfFilePath = defaultTtfFilePath;
    float fontDyFudge = 0.0f;
    const char *appTitle = "ImGui with Skia Backend";
    bool fullscreen = false;
    bool backdropFilter = false;
    bool sketchFilter = false;
    bool vsync = true;
    bool imguiNavKeyboard = false;
    bool imguiNavGamepad = false;
    bool imguiDocking = true;
    bool vectorCmd = true;
    const char *fontManager = nullptr;
    const char *fontManagerArg = nullptr;
    const char *skiaBackendType = "gl";
    const char *backgroundColorRGBA = "000000ff";

    CliOptions() = default;
    ~CliOptions() = default;

    void usage(const char *name, FILE *file) const;
    void parse(int argc,char **argv, FILE *logChannel);
};
