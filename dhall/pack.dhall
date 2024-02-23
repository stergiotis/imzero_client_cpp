let prelude = ./prelude.dhall
let lib = ./lib.dhall
let tarball = let T = {
    , name : Text
	, sourceTreeParts : List lib.sourceTreePart.Type
} in {
    , Type = T
    , default = {=}
}
--let here =  (./lib.dhall as Location)
--let renderLocation = \(l : prelude.Location.Type) -> 
--merge {
--    , prelude.Location.Type.Environment = \(p : Text) -> "\$${p}"
--    , prelude.Location.Type.Local = \(p : Text) -> p
--    , prelude.Location.Type.Remote = \(p : Text) -> p
--    , prelude.Location.Type.Missing = "__missing__"
--} l
let here = "${env:here as Text}"
--let renderLocation = \(v : Text) -> "${v}/"
let renderLocation = \(v : Text) -> ""

let dependenciesFileList = \(sps : List lib.sourceTreePart.Type) -> (prelude.List.map Text Text (\(p : Text) -> "${renderLocation here}${p}")
(prelude.List.concatMap lib.sourceTreePart.Type Text (\(sp : lib.sourceTreePart.Type) -> (sp.additionalDependants # sp.additionalIncludeDirs # sp.nonSourceObjs # sp.sources)) sps))
--let dependenciesFileList = \(sps : List lib.sourceTreePart.Type) -> 
--(prelude.List.concatMap lib.sourceTreePart.Type Text (\(sp : lib.sourceTreePart.Type) -> (sp.additionalDependants # sp.additionalIncludeDirs # sp.nonSourceObjs # sp.sources)) sps)
in {
    , dependenciesFileList
}