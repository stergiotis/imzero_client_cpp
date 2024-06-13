#pragma once
#include <cstdio>
auto constexpr defaultTtfFilePath = "./SauceCodeProNerdFontPropo-Regular.ttf";
struct CliOptions {
    const char *ttfFilePath = defaultTtfFilePath;
    float fontDyFudge = 0.0f;
    const char *fffiInFile = nullptr;
    const char *fffiOutFile = nullptr;
    const char *appTitle = "ImZeroClient";
    bool backdropFilter = true;
    bool sketchFilter = false;
    bool fffiInterpreter = true;
    bool vsync = true;
    bool imguiNavKeyboard = false;
    bool imguiNavGamepad = false;
    bool imguiDocking = true;
    const char *skiaBackendType = "gl";
    const char *backgroundColorRGBA = "000000ff";
    CliOptions() = default;
    ~CliOptions() = default;

    void usage(const char *name, FILE *file) const;
    void parse(int argc,char **argv, FILE *logChannel);
};
