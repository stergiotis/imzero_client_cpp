#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "../common/lib.sh"

link ../common/contrib/sdl3 contrib/sdl3
link ../common/contrib/tracy contrib/tracy
