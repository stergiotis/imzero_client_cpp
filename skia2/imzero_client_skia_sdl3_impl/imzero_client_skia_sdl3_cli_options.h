#pragma once
#include <cstdio>
#include <cstdint>
#include <imgui_skia_app_sdl3.h>

struct ImZeroCliOptions {
    bool fCoreDump = true;

    /* fffi interpreter */
    const char *fFffiInFile = nullptr;
    const char *fFffiOutFile = nullptr;
    bool fFffiInterpreter = false;

    /* video mode fields */
    const char *fVideoRawFramesFile = nullptr;
    uint32_t fVideoResolutionWidth = 0;
    uint32_t fVideoResolutionHeight = 0;
    const char *fVideoRawOutputFormat = nullptr;
    uint32_t fVideoExitAfterNFrames = 0;
    const char *fVideoUserInteractionEventsFile = nullptr;
    bool fVideoUserInteractionEventsAreBinary = false;

    ImZeroCliOptions() = default;
    ~ImZeroCliOptions() = default;

    void usage(const char *name, FILE *file) const;
    void version(FILE *file) const;
    void parse(int argc, const char **argv, FILE *logChannel, uint64_t &usedFlags);
    bool hasHelpFlag(int argc, const char** argv);
    void checkConsistency(int argc, const char **argv, FILE* logChannel, uint64_t usedFlags);

    ImGuiSkia::Driver::CliOptions fBaseOptions{};
};
