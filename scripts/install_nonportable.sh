#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

# linux
source /etc/os-release
./"install_${ID}_${VERSION_ID}.sh"
