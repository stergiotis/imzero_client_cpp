#include "imgui_skia_app_sdl3.h"

int main(int argc, char** argv) {
    CliOptions opts{};
    opts.parse(argc, argv, stderr, false);

    App app{};
    return app.Run(opts);
}
