#!/bin/bash
perf record -p $(pidof imgui_skia_exe) --call-graph dwarf sleep 10
