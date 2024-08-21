#!/bin/bash
rm -f core
coredumpctl -1 dump --output core
gdb ./imgui_exe core
