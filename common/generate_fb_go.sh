#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
flatc="../../contrib/flatbuffers/flatc"
"$flatc" -o imgui --go ../spec/ImZeroFB.fbs --reflect-types --reflect-names --go-namespace dto
rm -rf ../../boxer/public/imzero/dto/


for f in imgui/dto/*.go
do
    [ -f "$f" ] && mv "$f" "${f%go}out.go"
done

mv imgui/dto/ ../../boxer/public/imzero/
