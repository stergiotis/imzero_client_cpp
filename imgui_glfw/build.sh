#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
export IMZERO_CLIENT_CPP_ROOT=$(realpath $here)

../common/generate_links.sh
./generate_links.sh
./build_cpp.sh
