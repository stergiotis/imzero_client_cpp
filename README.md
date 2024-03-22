# ImZero Clients
Rendering of ImZero GUI library commands, generating of ImZero input commands.

See <a href="https://github.com/stergiotis/boxer">Boxer</a> for the corresponding go library to create applications acting as Imzero drivers/servers.

## Running the demo
Assuming Ubuntu Linux and go >1.21 installation:
```bash
git clone https://github.com/stergiotis/imzero_client_cpp
cd imzero_client_cpp
./scripts/install.sh
./scripts/install_ubuntu2204.sh
./imgui_glfw/build.sh
./scripts/demo.sh
```
Note that this clones the go library <a href="https://github.com/stergiotis/boxer">Boxer</a> and uses a `go.mod` directive to use it in the go build.
This ensures that the generated `dispatch.h` files match the corresponding fffi idl code in boxer.

## Developer Experience

### ImGui Assertions
The `imgui_glfw_dev/imgui_exe` binary can be used while developing. ImGui assertions will be reported on stderr. Note that depending on how the client binary has been started by the driving server program the output may not be visible. See `imgui_glfw_dev/run_pipe.sh` for an example.
Example of the error reported to stderr:
```
,------------------------[ ASSERTION ]-----------------------
| assertion raised: func PopStyleColor in /data/repo/imzero_client_cpp/imgui_glfw_dev/imgui/imgui.cpp:3140
| env variable IMZERO_ASSERT_BASE_PATH detected, executing sed -n '3137,3143'p '/data/repo/imzero_client_cpp/imgui_glfw_dev/imgui/imgui.cpp' 1>&2
    ImGuiContext& g = *GImGui;
    if (g.ColorStack.Size < count)
    {
        IM_ASSERT_USER_ERROR(g.ColorStack.Size > count, "Calling PopStyleColor() too many times!");
        count = g.ColorStack.Size;
    }
    while (count > 0)
`------------------------
```

## Contributing
Currently, no third-party contributions are accepted.

## License
The MIT License (MIT) 2024 - [Panos Stergiotis](https://github.com/stergiotis/). See [LICENSE](LICENSE) for the full license text.
