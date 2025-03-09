#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
version=$(git log -1  --pretty=format:"%H" | tr -d "\n")
extra=""
if [[ $(git diff --stat) != '' ]]; then
   extra+="_dirty"
fi
arch=$(uname --machine) # TODO use output of file or env variable?
fn="dist/imzero_client_skia_${version:0:8}${extra}_${arch}"
echo "dist=${fn}"
rm -rf "dist"
mkdir -p "$fn"
cp -rv bin "$fn"
cp -rv lib "$fn"
tar cvfJ "${fn}.tar.xz" "$fn"
