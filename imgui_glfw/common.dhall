let lib = ../dhall/lib.dhall
let sourceTreePartsRepo = ../dhall/sourceTreeParts.dhall
let mainPart = let dir = "." in lib.sourceTreePart::{
	, dir = dir
	, sources = [ "${dir}/main.cpp" ]
}
let sourceTreeParts = [
	, sourceTreePartsRepo.imgui
	, sourceTreePartsRepo.imguiBackendGlfw
	, sourceTreePartsRepo.imguiFreetype
	, sourceTreePartsRepo.render
	, sourceTreePartsRepo.marshalling
	, sourceTreePartsRepo.arena
	, sourceTreePartsRepo.widgets
	, sourceTreePartsRepo.imguiToggle
	, sourceTreePartsRepo.imguiImplot
	, sourceTreePartsRepo.imguiKnobs
	, sourceTreePartsRepo.imguiCoolbar
	, sourceTreePartsRepo.imguiFlamegraph
	, sourceTreePartsRepo.imguiTextedit
	, sourceTreePartsRepo.binding
	, mainPart
]
let cxx = "clang++"
let cppstd = "c++20"
in {
	, sourceTreeParts
	, cxx
	, cppstd
}
