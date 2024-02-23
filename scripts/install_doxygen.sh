#!/bin/bash
mkdir -p "$HOME/Downloads"
cd "$HOME/Downloads"

if [ ! -d "./doxygen-1.10.0" ]; then
   wget --quiet "https://www.doxygen.nl/files/doxygen-1.10.0.linux.bin.tar.gz"
   tar xfz "doxygen-1.10.0.linux.bin.tar.gz"
fi
