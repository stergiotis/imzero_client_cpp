#  Project Structure
The functionality is layered:
1) `ImGui with Hooks`;
2) `(1)` + (`FlatBuffer Vector Commands` + `Skia Text Measure`);
3) `(2)` + `Skia Renderer`;
4) `(3)` + `SDL3 Application Driver`.

Layer 1 aims to be drop-in compatible with ImGui. Code is contained in `./imgui_w_hooks_x.y.z`.

Layer 1,2, and 3 do not expose Skia in APIs. Code is contained in `./imgui_skia_impl`.

Layer 4 exposes Skia and SDL3 in the API. Code is contained in `./imgui_skia_driver_impl`.

An example application using layers 1 to 4 is contained in `./example_sdl3`.