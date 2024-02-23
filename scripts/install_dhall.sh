#!/bin/bash
mkdir -p "$HOME/Downloads"
cd "$HOME/Downloads"

if [ ! -d "./bin/dhall" ]; then
   wget --quiet "https://github.com/dhall-lang/dhall-haskell/releases/download/1.41.2/dhall-1.41.2-x86_64-Linux.tar.bz2"
   rm -f "./bin/dhall"
   tar xfj "dhall-1.41.2-x86_64-Linux.tar.bz2"
   mkdir -p "$HOME/.local/bin"
   mv "./bin/dhall" "$HOME/.local/bin"
fi
