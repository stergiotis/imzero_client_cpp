#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "generate_links_.sh"
