#pragma once
#include <cstdio>
#include <cstdint>
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
    bool vectorCmd = true;
    const char *fontManager = nullptr;
    const char *fontManagerArg = nullptr;
    const char *skiaBackendType = "gl";
    const char *backgroundColorRGBA = "000000ff";

    /* video mode fields */
    const char *videoRawFramesFile = nullptr;
    uint32_t videoResolutionWidth = 0;
    uint32_t videoResolutionHeight = 0;
    const char *videoRawOutputFormat = nullptr;
    uint32_t videoExitAfterNFrames = 0;
    const char *videoUserInteractionEventsFile = nullptr;
    bool videoUserInteractionEventsAreBinary = false;

    CliOptions() = default;
    ~CliOptions() = default;

    void usage(const char *name, FILE *file) const;
    void version(FILE *file) const;
    void parse(int argc,char **argv, FILE *logChannel);
};
