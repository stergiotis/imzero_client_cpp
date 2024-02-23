#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")

cd "$here"
./install_doxygen.sh

cd "$here"
./install_dhall.sh

cd "$here"
./install_3rd_party_git_repos.sh
