#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
./make.dhall
make clean
make -j
