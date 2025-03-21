#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/../../contrib/skia/"
echo "applying patches to skia m134"
sed -i -e "s,[a-z]+[.]googlesource[.]com/external/,,g" DEPS || true
