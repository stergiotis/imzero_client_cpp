let prelude = ./prelude.dhall
let lib = ./lib.dhall
let makefile = let T = {
    , cxx : Text
    , exe : Text
	, cxxflags : List Text
	, ldflags : List Text
	, sourceTreeParts : List lib.sourceTreePart.Type
	, generateDepFiles : Bool
} in {Type = T, default = {
	, cxxflags = [] : List Text
	, ldflags = [] : List Text
	, sourceTreeParts = [] : List lib.sourceTreePart.Type
	, generateDepFiles = False
}}
let definesToCxxflags = \(defines : List Text) -> (prelude.List.map Text Text (\(d : Text) -> "-D${d}") defines)
let includesToCxxflags = \(dirs : List Text) -> (prelude.List.map Text Text (\(d : Text) -> "-I${d}") dirs)
let makefileToText = \(m : makefile.Type) -> 
	let cxxflags = m.cxxflags
	            # (prelude.List.concatMap lib.sourceTreePart.Type Text 
				     (\(p : lib.sourceTreePart.Type) -> p.cxxflags.global 
					    # (includesToCxxflags ([p.dir] # p.additionalIncludeDirs))
					    # (definesToCxxflags p.defines.global)
						) m.sourceTreeParts)
	let ldflags = m.ldflags # (prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.ldflags.global) m.sourceTreeParts)
	let srcToObjFile = \(n : Text) -> prelude.Text.replace ".cpp" ".o" n
	let srcToDepFile = \(n : Text) -> n ++ ".dep"
	let sources = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.sources) m.sourceTreeParts
	let srcObjs = prelude.List.map Text Text srcToObjFile sources
	let srcDepFiles = prelude.List.map Text Text srcToDepFile sources
	let nonSourceObjs = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.nonSourceObjs) m.sourceTreeParts
	let composeBuildRule = \(p : lib.sourceTreePart.Type) ->
		let cf = p.cxxflags.local # (definesToCxxflags p.defines.local)
		in
		prelude.Text.concatMapSep "\n" Text (\(s : Text) -> 
			(prelude.Text.replace ".cpp" ".o" s)
			 ++ ": ${s} "
			 ++ (prelude.Text.concatSep " " p.additionalDependants)
			 ++ "\n" 
			 ++ "\t$(CXX) $(CXXFLAGS2) ${prelude.Text.concatSep " " (cf #
			 (if m.generateDepFiles then ["-MF", srcToDepFile s] else [] : List Text)
			 )} -c -o $@ $<"
			 ) p.sources
	let buildRules = prelude.Text.concatMapSep "\n\n" lib.sourceTreePart.Type composeBuildRule m.sourceTreeParts
	in
	""
	++ "CXX = ${m.cxx}\n"
	++ "EXE = ${m.exe}\n"
	++ "CXXFLAGS2 = ${prelude.Text.concatSep " " cxxflags}\n"
	++ "LDFLAGS = ${prelude.Text.concatSep " " ldflags}\n"
	++ "SOURCES = ${prelude.Text.concatSep " " sources}\n"
	++ "OBJS_NON_SRC = ${prelude.Text.concatSep " " nonSourceObjs}\n"
	++ "OBJS_SRC = ${prelude.Text.concatSep " " srcObjs}\n"
	++ "DEPFILES_SRC = ${prelude.Text.concatSep " " srcDepFiles}\n"
	++ "OBJS = $(OBJS_SRC) $(OBJS_NON_SRC)\n"
	++ "\n"
	++ "all: \$(EXE)\n"
	++ "\t@echo Build complete for all target\n"
	++ "\$(EXE): ${prelude.Text.concatSep " " srcObjs} $(OBJS_NON_SRC)\n"
	++ "\t\$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)\n"
	++ "deps: $(OBJS_SRC)\n"
	++ "\t@echo Build complete for deps target\n"
	++ "clean:\n"
	++ "\trm -f $(EXE) $(OBJS_SRC) $(DEPFILES_SRC)\n"
	++ buildRules
	++ "\n"
in {
    , makefile
    , makefileToText
}
