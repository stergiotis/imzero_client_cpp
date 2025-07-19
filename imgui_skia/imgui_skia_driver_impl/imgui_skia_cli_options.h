#pragma once
#include <cstdio>

namespace ImGuiSkia::Driver {
    auto constexpr defaultTtfFilePath = "./SauceCodeProNerdFontPropo-Regular.ttf";

    struct CliOptions {
        const char *fTtfFilePath = defaultTtfFilePath;
        float fFontDyFudge = 0.0f;
        float fFontScaleOverride = 1.0f;
        const char *fAppTitle = "ImZero";
        bool fFullscreen = false;
        int fInitialMainWindowWidth = -1;
        int fInitialMainWindowHeight = -1;
        bool fAllowMainWindowResize = true;
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
        const char *fExportBasePath = "/tmp/out";

        CliOptions() = default;
        ~CliOptions() = default;

        void usage(const char *name, FILE *file) const;
        bool hasHelpFlag(int argc, const char** argv);
        void parse(int argc, const char** argv, FILE* logChannel, uint64_t &usedFlags);
        void checkConsistency(int argc, const char **argv, FILE* logChannel, uint64_t usedFlags);
    };
    const char *findFlagValueDefault(FILE *logChannel, uint64_t &markUsed, int argc, const char **argv,const char *flag,const char *defaultValue);
    float findFlagValueDefaultFloat(FILE *logChannel, uint64_t &markUsed, int argc, const char **argv,const char *flag,const char *defaultValue);
    int64_t findFlagValueDefaultInt(FILE *logChannel, uint64_t &markUsed, int argc, const char **argv,const char *flag,const char *defaultValue);
    bool getBoolFlagValue(FILE *logChannel, uint64_t &markUsed, int argc, const char **argv,const char *flag,bool defaultValue);
}
