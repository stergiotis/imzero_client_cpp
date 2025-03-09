#include "imzero_client_skia_sdl3_app.h"
#include "imzero_client_skia_sdl3_cli_options.h"
#include "imgui_skia_cli_options.h"

#include <sys/prctl.h>

int main(const int argc, const char** argv) {
#ifdef TRACY_ENABLE
    ImGui::SetAllocatorFunctions(imZeroMemAlloc,imZeroMemFree,nullptr);
#endif

    ImZeroCliOptions opts{};
    uint64_t usedFlags = 0;
    if (opts.hasHelpFlag(argc, argv)) {
       opts.usage(argv[0],stderr);
       return 0;
    }
    opts.parse(argc, argv, stderr, usedFlags);
    opts.checkConsistency(argc, argv, stderr, usedFlags);

    if (0 > prctl(PR_SET_DUMPABLE, opts.fCoreDump ? 1 : 0)) {
        perror("unable to set prctl(PR_SET_DUMPABLE)");
        return 1;
    }

    ImGuiSkia::Driver::App app{};
    app.setup(opts.fBaseOptions);
    const int r = app.mainLoop();
    app.cleanup();
    return r;
}
