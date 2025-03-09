#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <bit>
#include <cmath>
#include "imgui_skia_cli_options.h"

static const char *findFlagValueDefault(FILE *logChannel, uint64_t &markUsed, int argc, char **argv,const char *flag,const char *defaultValue) {
    for(int i=1;i<argc;i++) {
        auto t = static_cast<uint64_t>(1) << (i);
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
static float findFlagValueDefaultFloat(FILE *logChannel, uint64_t &markUsed, int argc, char **argv,const char *flag,const char *defaultValue) {
    const char *v = findFlagValueDefault(logChannel, markUsed,argc,argv,flag,defaultValue);
    char *end;
    auto f = strtof(v, &end);
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
static int64_t findFlagValueDefaultInt(FILE *logChannel, uint64_t &markUsed, int argc, char **argv,const char *flag,const char *defaultValue) {
    const char *v = findFlagValueDefault(logChannel, markUsed,argc,argv,flag,defaultValue);
    char *end;
    auto f = strtol(v, &end,0);
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
static bool getBoolFlagValue(FILE *logChannel, uint64_t &markUsed, int argc, char **argv,const char *flag,bool defaultValue) {
    return strcmp(findFlagValueDefault(logChannel, markUsed, argc,argv,flag,defaultValue ? "on" : "off"),"on") == 0;
}
void CliOptions::usage(const char *name, FILE *file) const {
    fprintf(file,"%s\n", name);

    fprintf(file,"info flags:\n");
    fprintf(file,"   -help\n");

    fprintf(file,"general flags:\n");
    fprintf(file,"    -appTitle [title:%s]\n", fAppTitle);
    fprintf(file,"    -fullscreen [bool:%s]\n", fFullscreen ? "on" : "off");
    fprintf(file,"    -initialMainWindowWidth [int:%d]\n", fInitialMainWindowWidth);
    fprintf(file,"    -initialMainWindowHeight [int:%d]\n", fInitialMainWindowHeight);
    fprintf(file,"    -allowMainWindowResize [bool:%s]\n", fAllowMainWindowResize ? "on" : "off");

    fprintf(file,"graphics flags:\n");
    fprintf(file,"    -skiaBackendType [type:%s]    choices: raster,gl,vulkan\n",fSkiaBackendType);
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
    fprintf(file,"    -fontManager [name:%s]\n", fFontManager);
    fprintf(file,"    -fontManagerArg [arg:%s]\n", fFontManagerArg);
}
bool CliOptions::hasHelpFlag(const int argc, char **argv) {
    return argc > 1 && (strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"--help") == 0);
}
void CliOptions::parse(const int argc,char **argv,FILE *logChannel, const bool allowUnhandledFlags) {
    if(argc > 1) {
        if(strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"--help") == 0) {
            usage(argv[0],stderr);
            exit(0);
        }
    }
    uint64_t u = 0;

    /* general flags */
    fAppTitle = findFlagValueDefault(logChannel,u, argc, argv, "-appTitle", fAppTitle);
    fFullscreen = getBoolFlagValue(logChannel, u, argc, argv, "-fullscreen", fFullscreen);
    fInitialMainWindowWidth = static_cast<int>(findFlagValueDefaultInt(logChannel,u, argc, argv, "-initialMainWindowWidth", "-1"));
    fInitialMainWindowHeight = static_cast<int>(findFlagValueDefaultInt(logChannel,u, argc, argv, "-initialMainWindowHeight", "-1"));
    fAllowMainWindowResize = getBoolFlagValue(logChannel,u, argc, argv, "-allowMainWindowResize", fAllowMainWindowResize);

    /* graphics flags */
    fSkiaBackendType = findFlagValueDefault(logChannel,u, argc, argv, "-skiaBackendType", fSkiaBackendType);
    fVsync = getBoolFlagValue(logChannel,u,argc,argv,"-vsync",fVsync);
    fBackgroundColorRGBA = findFlagValueDefault(logChannel, u, argc, argv, "-backgroundColorRGBA", fBackgroundColorRGBA);
    if(strlen(fBackgroundColorRGBA) != 8) {
        fprintf(logChannel,"backgroundColorRGBA is not a valid rgba hex color: %s\n", fBackgroundColorRGBA);
        exit(1);
    }
    fBackdropFilter = getBoolFlagValue(logChannel,u, argc, argv, "-backdropFilter",fBackdropFilter);
    fSketchFilter = getBoolFlagValue(logChannel,u, argc, argv, "-sketchFilter",fSketchFilter);
    fVectorCmd = getBoolFlagValue(logChannel,u, argc, argv, "-vectorCmd",fVectorCmd);

    /* imgui flags */
    fImguiNavKeyboard = getBoolFlagValue(logChannel,u, argc, argv, "-imguiNavKeyboard",fImguiNavKeyboard);
    fImguiNavGamepad = getBoolFlagValue(logChannel,u, argc, argv, "-imguiNavGamepad",fImguiNavGamepad);
    fImguiDocking = getBoolFlagValue(logChannel,u, argc, argv, "-imguiDocking",fImguiDocking);

    /* font flags */
    fTtfFilePath = findFlagValueDefault(logChannel,u, argc, argv,"-ttfFilePath",defaultTtfFilePath);
    fFontDyFudge = findFlagValueDefaultFloat(logChannel,u, argc, argv, "-fontDyFudge", "0.0");
    if(std::isnan(fFontDyFudge) || fFontDyFudge < -10000.0f || fFontDyFudge > 10000.0f) {
        fprintf(logChannel,"implausible value for -fontDyFudge: %f\n", fFontDyFudge);
        exit(1);
    }
    fFontManager = findFlagValueDefault(logChannel,u, argc, argv, "-fontManager", fFontManager);
    fFontManagerArg = findFlagValueDefault(logChannel,u, argc, argv, "-fontManagerArg", fFontManagerArg);

    /* check consistency */
    if(!allowUnhandledFlags && std::popcount(u) != (argc-1)) {
        for(int i=1;i<argc;i++) {
            if(((static_cast<uint64_t>(1) << i) & u) == 0) {
                fprintf(logChannel,"found unhandled flag %s at position %d (1-based)\n",argv[i],i);
            }
        }
        exit(1);
    }
}