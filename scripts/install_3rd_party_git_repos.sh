#!/bin/bash
set -e
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
mkdir -p "../../contrib"
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
	if [ ! -z "$branch" ]; then
		git checkout -q "$branch"
	fi
	git fetch
	if [ ! -z "$commit" ]; then
		git checkout -q "$commit"
	fi
	echo -e '`------------------* done!'
	echo ""
}
function getContribRepo() {
	cd "$here/../../contrib"
	getRepo "$1" "$2" "$3" "$4"
}
function getMyRepo() {
	cd "$here/../../"
	getRepo "$1" "$2" "$3" "$4"
}
getContribRepo "https://github.com/emscripten-core/emsdk.git" "emsdk" "main" ""
getContribRepo "https://github.com/stergiotis/imgui.git" "imgui" "imzero_hooks_docking_1.91.9" ""
getContribRepo "https://github.com/stergiotis/imgui_club.git" "imgui_club" "main" "17510b03602c8b6c7ed48fd8fa29d9a75eb10dc6"
getContribRepo "https://github.com/stergiotis/imgui_flame-graph.git" "imgui_flame-graph" "master" "aae0bd9665d1379c511d0b8a64d3752301a327a2"
getContribRepo "https://github.com/stergiotis/imgui_ImCoolBar.git" "imgui_ImCoolBar" "master" "da3cd38eb3bf083b5a171d5b17e7ee13bf873a77"
getContribRepo "https://github.com/stergiotis/imgui_ImGuiColorTextEdit.git" "imgui_ImGuiColorTextEdit" "imzero" ""
getContribRepo "https://github.com/stergiotis/imgui_implot.git" "imgui_implot" "master" "47522f47054d33178e7defa780042bd2a06b09f9"
getContribRepo "https://github.com/stergiotis/imgui_imspinner.git" "imgui_imspinner" "master" "5e9b1c235a207a73df2b566aa7f373f0746126fe"
getContribRepo "https://github.com/stergiotis/imgui_knobs.git" "imgui_knobs" "main" "a0768e19300268d15d748b5f67bc30deda75e11a"
getContribRepo "https://github.com/stergiotis/imgui_toggle.git" "imgui_toggle" "main" "2b95aac1732058521d505d6e506f12aa1a331f90"
getContribRepo "https://github.com/dhall-lang/dhall-lang" "dhall-lang" "master" "v22.0.0"
getContribRepo "https://github.com/google/flatbuffers.git" "flatbuffers" "master" "129ef422e8a4e89d87a7216a865602673a6d0bf3"
getContribRepo "https://github.com/google/skia" "skia" "chrome/m134" ""
getContribRepo "https://github.com/wolfpld/tracy" "tracy" "master" "v0.11.1"
getContribRepo "https://github.com/libsdl-org/SDL.git" "sdl" "main" "d66483dfccfcdc4e03f719e318c7a76f963f22d9"
getContribRepo "https://github.com/phoboslab/qoi.git" "qoi" "master" "bf7b41c2ff3f24a2031193b62aa76d35e8842b5a"
getMyRepo "https://github.com/stergiotis/boxer.git" "boxer" "main" ""
