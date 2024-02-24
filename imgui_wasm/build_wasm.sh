#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

function installEmsdk() {
	./emsdk install latest
	./emsdk activate latest
}

cd "../../contrib/emsdk/"
#installEmsdk
source ./emsdk_env.sh
cd "$here"

make clean
make -j
