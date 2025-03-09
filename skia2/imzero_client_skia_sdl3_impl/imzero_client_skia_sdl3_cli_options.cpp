#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "buildinfo.gen.h"
#include "imzero_client_skia_sdl3_cli_options.h"
#include "imgui_skia_cli_options.h"

void ImZeroCliOptions::version(FILE *file) const {
    fprintf(file, "   version: git-commit=\"%s\",dirty=\"%s\"\n\n",buildinfo::gitCommit,buildinfo::gitDirty ? "yes" : "no");
}
bool ImZeroCliOptions::hasHelpFlag(const int argc, const char **argv) {
    return fBaseOptions.hasHelpFlag(argc, argv);
}
void ImZeroCliOptions::usage(const char *name, FILE *file) const {
    fBaseOptions.usage(name, file);

    fprintf(file,"fffi flags:\n");
    fprintf(file,"    -fffiInterpreter [bool:%s]\n",fFffiInterpreter ? "on" : "off");
    fprintf(file,"    -fffiInFile [path:%s]\n",fFffiInFile == nullptr ? "<stdin>" : fFffiInFile);
    fprintf(file,"    -fffiOutFile [path:%s]\n",fFffiOutFile == nullptr ? "<stdout>" : fFffiOutFile);

    fprintf(file,"video mode flags:\n");
    fprintf(file,"    -videoUserInteractionEventsInFile [path:%s]\n", fVideoUserInteractionEventsFile);
    fprintf(file,"    -videoUserInteractionEventsAreBinary [bool:%s]\n", fVideoUserInteractionEventsAreBinary ? "on" : "off");
    fprintf(file,"    -videoRawFramesFile [path:%s]\n", fVideoRawFramesFile);
    fprintf(file,"    -videoRawOutputFormat [format:%s]\n", fVideoRawOutputFormat);
    fprintf(file,"    -videoResolutionWidth [int:%u]\n", fVideoResolutionWidth);
    fprintf(file,"    -videoResolutionHeight [int:%u]\n", fVideoResolutionHeight);
    fprintf(file,"    -videoExitAfterNFrames [int:%u]\n", fVideoExitAfterNFrames);
}
void ImZeroCliOptions::parse(int argc, const char **argv,FILE *logChannel, uint64_t &usedFlags) {
    fFffiInFile = ImGuiSkia::Driver::findFlagValueDefault(logChannel,usedFlags, argc, argv, "-fffiInFile", fFffiInFile);
    fFffiOutFile = ImGuiSkia::Driver::findFlagValueDefault(logChannel,usedFlags, argc, argv, "-fffiOutFile", fFffiOutFile);
    fFffiInterpreter = ImGuiSkia::Driver::getBoolFlagValue(logChannel,usedFlags,argc,argv,"-fffiInterpreter",fFffiInterpreter);
    fCoreDump = ImGuiSkia::Driver::getBoolFlagValue(logChannel,usedFlags, argc, argv, "-coreDump",fCoreDump);
    fVideoUserInteractionEventsAreBinary = ImGuiSkia::Driver::getBoolFlagValue(logChannel,usedFlags,argc,argv,"-videoUserInteractionEventsAreBinary",fVideoUserInteractionEventsAreBinary);

    fVideoRawFramesFile = ImGuiSkia::Driver::findFlagValueDefault(logChannel,usedFlags, argc, argv,"-videoRawFramesFile",fVideoRawFramesFile);
    fVideoResolutionWidth = static_cast<uint32_t>(ImGuiSkia::Driver::findFlagValueDefaultInt(logChannel, usedFlags, argc, argv, "-videoResolutionWidth", "1920"));
    fVideoResolutionHeight = static_cast<uint32_t>(ImGuiSkia::Driver::findFlagValueDefaultInt(logChannel, usedFlags, argc, argv, "-videoResolutionHeight", "1080"));
    fVideoExitAfterNFrames = static_cast<uint32_t>(ImGuiSkia::Driver::findFlagValueDefaultInt(logChannel, usedFlags, argc, argv, "-videoExitAfterNFrames", "0"));
    fVideoRawOutputFormat = ImGuiSkia::Driver::findFlagValueDefault(logChannel, usedFlags, argc, argv, "-videoRawOutputFormat", "qoi");
    fVideoUserInteractionEventsFile = ImGuiSkia::Driver::findFlagValueDefault(logChannel, usedFlags, argc, argv, "-videoUserInteractionEventsFile", fVideoUserInteractionEventsFile);

    fBaseOptions.parse(argc,argv,logChannel,usedFlags);
}
void ImZeroCliOptions::checkConsistency(const int argc,const char **argv, FILE *logChannel, const uint64_t usedFlags) {
    fBaseOptions.checkConsistency(argc,argv,logChannel,usedFlags);
}
