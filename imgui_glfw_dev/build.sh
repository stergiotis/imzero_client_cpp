#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
../common/generate_links.sh
./generate_links.sh
./build_cpp.sh
