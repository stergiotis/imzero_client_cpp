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
getContribRepo "https://github.com/stergiotis/imgui.git" "imgui" "imzero_hooks_docking" ""
getContribRepo "https://github.com/stergiotis/imgui_club.git" "imgui_club" "main" "ea49dd3c6803088d50b496e3fe981501543b7cbc"
getContribRepo "https://github.com/stergiotis/imgui_flame-graph.git" "imgui_flame-graph" "master" "aae0bd9665d1379c511d0b8a64d3752301a327a2"
getContribRepo "https://github.com/stergiotis/imgui_ImCoolBar.git" "imgui_ImCoolBar" "master" "da3cd38eb3bf083b5a171d5b17e7ee13bf873a77"
getContribRepo "https://github.com/stergiotis/imgui_ImGuiColorTextEdit.git" "imgui_ImGuiColorTextEdit" "imzero" "3e7734d2d8a5c94ffa3fce0e39ff9a573ed4a9d4"
getContribRepo "https://github.com/stergiotis/imgui_implot.git" "imgui_implot" "master" "065acc3319f0422479c0fed5a5edccd0f563729f"
getContribRepo "https://github.com/stergiotis/imgui_imspinner.git" "imgui_imspinner" "master" "4287e464353c05c1e2d8633d091fe0f647de2648"
getContribRepo "https://github.com/stergiotis/imgui_knobs.git" "imgui_knobs" "main" "4f207526f9ef036a0aff7edfaad92cfbe12d987a"
getContribRepo "https://github.com/stergiotis/imgui_toggle.git" "imgui_toggle" "main" "d8d22c0f41bc4923e224bdf5a2fae8ea6a4aab1a"
getContribRepo "https://github.com/dhall-lang/dhall-lang" "dhall-lang" "master" "v22.0.0"
getContribRepo "https://github.com/google/flatbuffers.git" "flatbuffers" "master" "129ef422e8a4e89d87a7216a865602673a6d0bf3"
getContribRepo "https://github.com/google/skia" "skia" "chrome/m124" ""
getContribRepo "https://github.com/wolfpld/tracy" "tracy" "master" "0d5bd53be393b590da9c7b29079d919d488412c1"
getContribRepo "https://github.com/libsdl-org/SDL.git" "sdl" "main" "9ea0a837aebeb474441eef13c039e2890cfbae9e"
getContribRepo "https://github.com/phoboslab/qoi.git" "qoi" "master" "bf7b41c2ff3f24a2031193b62aa76d35e8842b5a"
getMyRepo "https://github.com/stergiotis/boxer.git" "boxer" "main" ""
