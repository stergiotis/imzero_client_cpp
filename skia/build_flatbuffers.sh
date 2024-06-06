#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
set -ev
cd ../../contrib/flatbuffers
cmake -G "Unix Makefiles" .
make -j
