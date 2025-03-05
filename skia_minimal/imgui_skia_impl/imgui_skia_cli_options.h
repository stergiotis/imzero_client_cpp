#pragma once
#include <cstdio>
auto constexpr defaultTtfFilePath = "./SauceCodeProNerdFontPropo-Regular.ttf";

struct CliOptions {
    const char *fTtfFilePath = defaultTtfFilePath;
    float fFontDyFudge = 0.0f;
    const char *fAppTitle = "ImGui with Skia Backend";
    bool fFullscreen = false;
    bool fBackdropFilter = false;
    bool fSketchFilter = false;
    bool fVsync = true;
    bool fImguiNavKeyboard = false;
    bool fImguiNavGamepad = false;
    bool fImguiDocking = true;
    bool fVectorCmd = true;
    const char *fFontManager = nullptr;
    const char *fFontManagerArg = nullptr;
    const char *fSkiaBackendType = "gl";
    const char *fBackgroundColorRGBA = "000000ff";

    CliOptions() = default;
    ~CliOptions() = default;

    void usage(const char *name, FILE *file) const;
    bool hasHelpFlag(int argc, char** argv);
    void parse(int argc,char **argv, FILE *logChannel,bool allowUnhandledFlags);
};
