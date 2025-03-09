#include "imgui_skia_app_sdl3.h"

int main(int argc, char** argv) {
    CliOptions opts{};
    opts.parse(argc, argv, stderr, false);

    App app{};
    app.setup(opts);
    return app.mainLoop();
}
