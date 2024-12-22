# Developer Experience

## ImGui Assertions
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
