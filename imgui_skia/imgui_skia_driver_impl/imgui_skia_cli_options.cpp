#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <bit>
#include <cmath>
#include "imgui_skia_cli_options.h"

const char *ImGuiSkia::Driver::findFlagValueDefault(FILE *logChannel, uint64_t &markUsed, int argc, const char **argv,const char *flag,const char *defaultValue) {
    for(int i=1;i<argc;i++) {
        auto const t = static_cast<uint64_t>(1) << (i);
        if(((markUsed & t) == 0) && strcmp(argv[i],flag) == 0) {
            if(i == argc) {
                fprintf(logChannel, "expecting argument for flag %s\n", flag);
                exit(1);
            } else {
                markUsed |= t;
                markUsed |= (t << 1);
                return argv[i+1];
            }
        }
    }
    return defaultValue;
}
float ImGuiSkia::Driver::findFlagValueDefaultFloat(FILE *logChannel, uint64_t &markUsed, int argc, const char **argv,const char *flag,const char *defaultValue) {
    const char *v = findFlagValueDefault(logChannel, markUsed,argc,argv,flag,defaultValue);
    char *end;
    const auto f = strtof(v, &end);
    if(f == 0.0f && end == v) {
        fprintf(stderr,"unable to parse argument for flag %s as float", flag);
        exit(1);
    }
    if(end[0] != '\0') {
        fprintf(stderr,"unable to parse argument for flag %s as float: trailing data found '%s'", flag, end);
        exit(1);
    }
    return f;
}
int64_t ImGuiSkia::Driver::findFlagValueDefaultInt(FILE *logChannel, uint64_t &markUsed, int argc, const char **argv,const char *flag,const char *defaultValue) {
    const char *v = findFlagValueDefault(logChannel, markUsed,argc,argv,flag,defaultValue);
    char *end;
    const auto f = strtol(v, &end,0);
    if(f == 0 && end == v) {
        fprintf(stderr,"unable to parse argument for flag %s as integer", flag);
        exit(1);
    }
    if(end[0] != '\0') {
        fprintf(stderr,"unable to parse argument for flag %s as integer: trailing data found '%s'", flag, end);
        exit(1);
    }
    return f;
}
bool ImGuiSkia::Driver::getBoolFlagValue(FILE *logChannel, uint64_t &markUsed, const int argc, const char **argv,const char *flag, const bool defaultValue) {
    return strcmp(findFlagValueDefault(logChannel, markUsed, argc,argv,flag,defaultValue ? "on" : "off"),"on") == 0;
}
void ImGuiSkia::Driver::CliOptions::usage(const char *name, FILE *file) const {
    fprintf(file,"%s\n", name);

    fprintf(file,"info flags:\n");
    fprintf(file,"   -help\n");

    fprintf(file,"general flags:\n");
    fprintf(file,"    -appTitle [title:%s]\n", fAppTitle);
    fprintf(file,"    -fullscreen [bool:%s]\n", fFullscreen ? "on" : "off");
    fprintf(file,"    -initialMainWindowWidth [int:%d]\n", fInitialMainWindowWidth);
    fprintf(file,"    -initialMainWindowHeight [int:%d]\n", fInitialMainWindowHeight);
    fprintf(file,"    -allowMainWindowResize [bool:%s]\n", fAllowMainWindowResize ? "on" : "off");
    fprintf(file,"    -exportBasePath [path:%s]\n", fExportBasePath);

    fprintf(file,"graphics flags:\n");
    //fprintf(file,"    -skiaBackendType [type:%s]    choices: raster,gl,vulkan\n",fSkiaBackendType); // TODO not implemented
    fprintf(file,"    -vsync [bool:%s]\n",fVsync ? "on" : "off");
    fprintf(file,"    -backgroundColorRGBA [hexrgba:%s]   example: 1199ffaa\n",fBackgroundColorRGBA);
    fprintf(file,"    -backdropFilter [bool:%s]\n", fBackdropFilter ? "on" : "off");
    fprintf(file,"    -sketchFilter [bool:%s]\n", fSketchFilter ? "on" : "off");
    fprintf(file,"    -vectorCmd [bool:%s]   on: intercept ImGui DrawList draw commands and replay them on client (e.g. skia)\n", fVectorCmd ? "on" : "off");

    fprintf(file,"imgui flags:\n");
    fprintf(file,"    -imguiNavKeyboard [bool:%s]\n", fImguiNavKeyboard ? "on" : "off");
    fprintf(file,"    -imguiNavGamepad [bool:%s]\n", fImguiNavGamepad ? "on" : "off");
    fprintf(file,"    -imguiDocking [bool:%s]\n", fImguiDocking ? "on" : "off");

    fprintf(file,"font flags:\n");
    fprintf(file,"    -ttfFilePath [path:%s]\n", fTtfFilePath);
    fprintf(file,"    -fontDyFudge [float:%f]\n", fFontDyFudge);
    fprintf(file,"    -fontScaleOverride [float:%f]\n", fFontScaleOverride);
    fprintf(file,"    -fontManager [name:%s] valid names (platform depenent) \"fontconfig\", \"directory\", \"directwrite\"\n", fFontManager);
    fprintf(file,"    -fontManagerArg [arg:%s]\n", fFontManagerArg);
}
bool ImGuiSkia::Driver::CliOptions::hasHelpFlag(const int argc, const char **argv) {
    return argc > 1 && (strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"--help") == 0);
}
void ImGuiSkia::Driver::CliOptions::parse(const int argc,const char **argv,FILE *logChannel, uint64_t &usedFlags) {
    /* general flags */
    fAppTitle = findFlagValueDefault(logChannel,usedFlags, argc, argv, "-appTitle", fAppTitle);
    fFullscreen = getBoolFlagValue(logChannel, usedFlags, argc, argv, "-fullscreen", fFullscreen);
    fInitialMainWindowWidth = static_cast<int>(findFlagValueDefaultInt(logChannel,usedFlags, argc, argv, "-initialMainWindowWidth", "-1"));
    fInitialMainWindowHeight = static_cast<int>(findFlagValueDefaultInt(logChannel,usedFlags, argc, argv, "-initialMainWindowHeight", "-1"));
    fAllowMainWindowResize = getBoolFlagValue(logChannel,usedFlags, argc, argv, "-allowMainWindowResize", fAllowMainWindowResize);
    fExportBasePath = findFlagValueDefault(logChannel,usedFlags, argc, argv, "-exportBasePath", fExportBasePath);

    /* graphics flags */
    fSkiaBackendType = findFlagValueDefault(logChannel,usedFlags, argc, argv, "-skiaBackendType", fSkiaBackendType);
    fVsync = getBoolFlagValue(logChannel,usedFlags,argc,argv,"-vsync",fVsync);
    fBackgroundColorRGBA = findFlagValueDefault(logChannel, usedFlags, argc, argv, "-backgroundColorRGBA", fBackgroundColorRGBA);
    if(strlen(fBackgroundColorRGBA) != 8) {
        fprintf(logChannel,"backgroundColorRGBA is not a valid rgba hex color: %s\n", fBackgroundColorRGBA);
        exit(1);
    }
    fBackdropFilter = getBoolFlagValue(logChannel,usedFlags, argc, argv, "-backdropFilter",fBackdropFilter);
    fSketchFilter = getBoolFlagValue(logChannel,usedFlags, argc, argv, "-sketchFilter",fSketchFilter);
    fVectorCmd = getBoolFlagValue(logChannel,usedFlags, argc, argv, "-vectorCmd",fVectorCmd);

    /* imgui flags */
    fImguiNavKeyboard = getBoolFlagValue(logChannel,usedFlags, argc, argv, "-imguiNavKeyboard",fImguiNavKeyboard);
    fImguiNavGamepad = getBoolFlagValue(logChannel,usedFlags, argc, argv, "-imguiNavGamepad",fImguiNavGamepad);
    fImguiDocking = getBoolFlagValue(logChannel,usedFlags, argc, argv, "-imguiDocking",fImguiDocking);

    /* font flags */
    fTtfFilePath = findFlagValueDefault(logChannel,usedFlags, argc, argv,"-ttfFilePath",defaultTtfFilePath);
    fFontDyFudge = findFlagValueDefaultFloat(logChannel,usedFlags, argc, argv, "-fontDyFudge", "0.0");
    if(std::isnan(fFontDyFudge) || fFontDyFudge < -10000.0f || fFontDyFudge > 10000.0f) {
        fprintf(logChannel,"implausible value for -fontDyFudge: %f\n", fFontDyFudge);
        exit(1);
    }
    fFontScaleOverride = findFlagValueDefaultFloat(logChannel,usedFlags, argc, argv, "-fontScaleOverride", "1.0");
    if(std::isnan(fFontDyFudge) || fFontDyFudge < -10000.0f || fFontDyFudge > 10000.0f) {
        fprintf(logChannel,"implausible value for -fontScaleOverride: %f\n", fFontScaleOverride);
        exit(1);
    }
    fFontManager = findFlagValueDefault(logChannel,usedFlags, argc, argv, "-fontManager", fFontManager);
    fFontManagerArg = findFlagValueDefault(logChannel,usedFlags, argc, argv, "-fontManagerArg", fFontManagerArg);
}
void ImGuiSkia::Driver::CliOptions::checkConsistency(const int argc,const char **argv, FILE *logChannel, const uint64_t usedFlags) {
    if(std::popcount(usedFlags) != (argc-1)) {
        for(int i=1;i<argc;i++) {
            if(((static_cast<uint64_t>(1) << i) & usedFlags) == 0) {
                fprintf(logChannel,"found unhandled flag %s at position %d (1-based)\n",argv[i],i);
            }
        }
        exit(1);
    }
}
