#!/bin/bash
set -e
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
mkdir -p ../../contrib

function getRepo() {
	url="$1"
	dir="$2"
	branch="$3"
	commit="$4"
	echo -e "\n,--------[ checking out $dir branch $branch commit $commit ]-------"
	cd ../../contrib
	if [ ! -d "$dir" ]; then
		git clone "$url" "$dir"
	fi
	if ! cd "$dir" ; then
	   echo "directory \"$dir\" not found after cloning \"$url\""
	   exit 1
	fi
	git clean -f -d
	git reset --hard
	if [ ! -z "$branch" ]; then
		git checkout -q "$branch"
	fi
	git pull
	if [ ! -z "$commit" ]; then
		git checkout -q "$commit"
	fi
	echo -e '`------------------* done!'
	echo ""
}
getRepo "https://github.com/emscripten-core/emsdk.git" "emsdk" "main" ""
getRepo "https://github.com/stergiotis/imgui.git" "imgui" "docking" "54ef4092a92f777ee6c855b08875e37a4e282b45"
getRepo "https://github.com/stergiotis/imgui_club.git" "imgui_club" "main" "ea49dd3c6803088d50b496e3fe981501543b7cbc"
getRepo "https://github.com/stergiotis/imgui_flame-graph.git" "imgui_flame-graph" "master" "aae0bd9665d1379c511d0b8a64d3752301a327a2"
getRepo "https://github.com/stergiotis/imgui_ImCoolBar.git" "imgui_ImCoolBar" "master" "da3cd38eb3bf083b5a171d5b17e7ee13bf873a77"
getRepo "https://github.com/stergiotis/imgui_ImGuiColorTextEdit.git" "imgui_ImGuiColorTextEdit" "master" "0a88824f7de8d0bd11d8419066caa7d3469395c4"
getRepo "https://github.com/stergiotis/imgui_implot.git" "imgui_implot" "master" "065acc3319f0422479c0fed5a5edccd0f563729f"
getRepo "https://github.com/stergiotis/imgui_imspinner.git" "imgui_imspinner" "master" "4287e464353c05c1e2d8633d091fe0f647de2648"
getRepo "https://github.com/stergiotis/imgui_knobs.git" "imgui_knobs" "main" "4f207526f9ef036a0aff7edfaad92cfbe12d987a"
getRepo "https://github.com/stergiotis/imgui_toggle.git" "imgui_toggle" "main" "d8d22c0f41bc4923e224bdf5a2fae8ea6a4aab1a"
getRepo "https://github.com/stergiotis/boxer.git" "boxer" "main" ""
