#!/bin/bash
set -ev
set -o pipefail
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

# linux
source /etc/os-release
version_id=$(echo "$VERSION_ID" | grep -o -E "[0-9]+[.]?[0-9]*" | head -n 1)
."/nonportable/${ID}_${version_id}.sh"
