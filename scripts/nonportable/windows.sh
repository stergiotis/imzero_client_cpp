#!/bin/bash
set -ev
curl -v -L "https://github.com/dhall-lang/dhall-haskell/releases/download/1.42.2/dhall-1.42.2-x86_64-windows.zip" -o dhall.zip
mkdir -p dhallinstall
mkdir -p "$HOME/bin"
cd dhallinstall || exit 1
unzip ../dhall.zip
mv bin/dhall.exe "$HOME/bin/dhall.exe"
cd - || exit 1
rm -f dhall.zip
rm -rf dhallinstall
