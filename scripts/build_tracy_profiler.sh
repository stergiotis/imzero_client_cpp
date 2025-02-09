#!/bin/bash
set -e
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/../../contrib/tracy"
rm -rf profiler/build
cmake -B profiler/build -S profiler -DCMAKE_BUILD_TYPE=Release -DNO_PARALLEL_STL=true
cmake --build profiler/build --config Release --parallel
