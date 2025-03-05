#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/contrib/flatbuffers" || exit 1
cmake -G "Unix Makefiles" .
make -j
