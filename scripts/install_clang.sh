#!/bin/bash
url="https://github.com/llvm/llvm-project/releases/download/llvmorg-18.1.8/"
fn="clang+llvm-18.1.8-x86_64-linux-gnu-ubuntu-18.04"
mkdir -p "$HOME/Downloads"
cd "$HOME/Downloads"

if [ ! -d "./$fn" ]; then
   wget --quiet "$url/$fn.tar.xz"
   tar xfJ "$fn.tar.xz"
fi
