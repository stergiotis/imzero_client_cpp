#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
rm -f main_go
source tags.sh
go build -tags "$build_tags" -o main_go ../../boxer/public/imzero/main.go
