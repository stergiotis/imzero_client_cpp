#include "imgui_skia_app_sdl3.h"

int main(int argc, const char** argv) {
    ImGuiSkia::Driver::CliOptions opts{};
    uint64_t usedFlags = 0;
    if(opts.hasHelpFlag(argc, argv)) {
        opts.usage(argv[0],stderr);
        return 0;
    }
    opts.parse(argc, argv, stderr, usedFlags);
    opts.checkConsistency(argc, argv, stderr, usedFlags);

    ImGuiSkia::Driver::App app{};
    app.setup(opts);
    app.completeFontSetup();
    const int r = app.mainLoop();
    app.cleanup();
    return r;
}
