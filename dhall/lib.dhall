let vsCodeConfiguration = let T = {
		, name : Text
		, compilerPath: Text
		, compilerArgs : List Text
		, intelliSenseMode : Text
		, includePath : List Text
		, defines : List Text
		, cStandard : Text
		, cppStandard : Text
		, configurationProvider : Text
		, forcedInclude : List Text
		, compileCommands: Text
		, mergeConfigurations : Bool
		, browse : {
			path: List Text,
			limitSymbolsToIncludedHeaders : Bool,
			databaseFilename : Text
		}
	}
	let schema = {Type = T, default = {=}}
	in schema
let vsCodeProperties = let T = {
		, version : Natural
		, enableConfigurationSquiggles : Bool
		, configurations : List vsCodeConfiguration.Type
	}
	let schema = {Type = T, default = {=}}
	in schema
let sourceTreePart = let T = {
	, dir : Text
	, name : Text
	, sources : List Text
	, additionalDependants : List Text
	, additionalIncludeDirs : List Text
	, defines : {
		, local : List Text
		, global : List Text
	}
	, cxxflags : {
		, local : List Text
		, global : List Text
	}
	, ldflags : {
		global : List Text
	}
	, nonSourceObjs : List Text
}
	let schema = {Type = T, default = {
		, additionalDependants = [] : List Text
		, cxxflags = {local = [] : List Text, global = [] : List Text}
		, ldflags = {global = [] : List Text}
		, nonSourceObjs = [] : List Text
		, additionalIncludeDirs = [] : List Text
		, defines = {, local = [] : List Text, global = [] : List Text}
	}}
	in schema
in
{
	, vsCodeProperties
	, vsCodeConfiguration
	, sourceTreePart
}