#!/bin/bash
set -e
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
mkdir -p "contrib"
export LC_ALL=C
function getRepo() {
	url="$1"
	dir="$2"
	branch="$3"
	commit="$4"
	echo -e "\n,--------[ checking out $dir branch $branch commit $commit ]-------"
	if [ ! -d "$dir" ]; then
		git clone "$url" "$dir"
	fi
	if ! cd "$dir" ; then
	   echo "directory \"$dir\" not found after cloning \"$url\""
	   exit 1
	fi
	git remote set-url origin "$url"
	git config pull.rebase false
	defaultBranch=$(git remote show origin | sed -n '/HEAD branch/s/.*: //p')
	git clean -f -d
	git reset --hard
	git merge --abort || true
	git checkout HEAD
	git clean -f -d
	git reset --hard
	git fetch origin "$defaultBranch"
	if [ -n "$branch" ]; then
		git checkout -q "$branch"
	fi
	git fetch
	if [ -n "$commit" ]; then
		git checkout -q "$commit"
	fi
	echo -e '`------------------* done!'
	echo ""
}
function getContribRepo() {
	cd "$here/contrib"
	getRepo "$1" "$2" "$3" "$4"
}
getContribRepo "https://github.com/stergiotis/imgui.git" "imgui" "imzero_hooks_docking_1.91.9" ""
getContribRepo "https://github.com/dhall-lang/dhall-lang" "dhall-lang" "master" "v22.0.0"
getContribRepo "https://github.com/google/flatbuffers.git" "flatbuffers" "master" "129ef422e8a4e89d87a7216a865602673a6d0bf3"
getContribRepo "https://github.com/google/skia" "skia" "chrome/m124" ""
getContribRepo "https://github.com/wolfpld/tracy" "tracy" "master" "v0.11.1"
getContribRepo "https://github.com/libsdl-org/SDL.git" "sdl" "main" "7c12c63f63be522af2ad8e8216ea7b2786aa42a0"