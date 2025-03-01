#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <bit>
#include <cmath>
#include "cliOptions.h"

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
static bool getBoolFlagValue(FILE *logChannel, uint64_t &markUsed, int argc, char **argv,const char *flag,bool defaultValue) {
    return strcmp(findFlagValueDefault(logChannel, markUsed, argc,argv,flag,defaultValue ? "on" : "off"),"on") == 0;
}
void CliOptions::usage(const char *name, FILE *file) const {
    fprintf(file,"%s\n", name);

    fprintf(file,"info flags:\n");
    fprintf(file,"   -help\n");

    fprintf(file,"general flags:\n");
    fprintf(file,"    -appTitle [title:%s]\n", appTitle);
    fprintf(file,"    -fullscreen [bool:%s]\n", fullscreen ? "on" : "off");

    fprintf(file,"graphics flags:\n");
    fprintf(file,"    -skiaBackendType [type:%s]    choices: raster,gl,vulkan\n",skiaBackendType);
    fprintf(file,"    -vsync [bool:%s]\n",vsync ? "on" : "off");
    fprintf(file,"    -backgroundColorRGBA [hexrgba:%s]   example: 1199ffaa\n",backgroundColorRGBA);
    fprintf(file,"    -backdropFilter [bool:%s]\n", backdropFilter ? "on" : "off");
    fprintf(file,"    -sketchFilter [bool:%s]\n", sketchFilter ? "on" : "off");
    fprintf(file,"    -vectorCmd [bool:%s]   on: intercept ImGui DrawList draw commands and replay them on client (e.g. skia)\n", vectorCmd ? "on" : "off");

    fprintf(file,"imgui flags:\n");
    fprintf(file,"    -imguiNavKeyboard [bool:%s]\n", imguiNavKeyboard ? "on" : "off");
    fprintf(file,"    -imguiNavGamepad [bool:%s]\n", imguiNavGamepad ? "on" : "off");
    fprintf(file,"    -imguiDocking [bool:%s]\n", imguiDocking ? "on" : "off");

    fprintf(file,"font flags:\n");
    fprintf(file,"    -ttfFilePath [path:%s]\n",ttfFilePath);
    fprintf(file,"    -fontDyFudge [float:%f]\n", fontDyFudge);
    fprintf(file,"    -fontManager [name:%s]\n", fontManager);
    fprintf(file,"    -fontManagerArg [arg:%s]\n", fontManagerArg);
}
void CliOptions::parse(int argc,char **argv,FILE *logChannel) {
    if(argc > 1) {
        if(strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"--help") == 0) {
            usage(argv[0],stderr);
            exit(0);
        }
    }
    uint64_t u = 0;

    ttfFilePath = findFlagValueDefault(logChannel,u, argc, argv,"-ttfFilePath",defaultTtfFilePath);
    appTitle = findFlagValueDefault(logChannel,u, argc, argv, "-appTitle", appTitle);
    fullscreen = getBoolFlagValue(logChannel, u, argc, argv, "-fullscreen", fullscreen);
    skiaBackendType = findFlagValueDefault(logChannel,u, argc, argv, "-skiaBackendType", skiaBackendType);
    backgroundColorRGBA = findFlagValueDefault(logChannel, u, argc, argv, "-backgroundColorRGBA", backgroundColorRGBA);
    if(strlen(backgroundColorRGBA) != 8) {
        fprintf(logChannel,"backgroundColorRGBA is not a valid rgba hex color: %s\n", backgroundColorRGBA);
        exit(1);
    }
    fontManager = findFlagValueDefault(logChannel,u, argc, argv, "-fontManager", fontManager);
    fontManagerArg = findFlagValueDefault(logChannel,u, argc, argv, "-fontManagerArg", fontManagerArg);

    fontDyFudge = findFlagValueDefaultFloat(logChannel,u, argc, argv, "-fontDyFudge", "0.0");
    if(std::isnan(fontDyFudge) || fontDyFudge < -10000.0f || fontDyFudge > 10000.0f) {
        fprintf(logChannel,"implausible value for -fontDyFudge: %f\n", fontDyFudge);
        exit(1);
    }

    vsync = getBoolFlagValue(logChannel,u,argc,argv,"-vsync",vsync);
    backdropFilter = getBoolFlagValue(logChannel,u, argc, argv, "-backdropFilter",backdropFilter);
    sketchFilter = getBoolFlagValue(logChannel,u, argc, argv, "-sketchFilter",sketchFilter);
    imguiNavKeyboard = getBoolFlagValue(logChannel,u, argc, argv, "-imguiNavKeyboard",imguiNavKeyboard);
    imguiNavGamepad = getBoolFlagValue(logChannel,u, argc, argv, "-imguiNavGamepad",imguiNavGamepad);
    imguiDocking = getBoolFlagValue(logChannel,u, argc, argv, "-imguiDocking",imguiDocking);
    vectorCmd = getBoolFlagValue(logChannel,u, argc, argv, "-vectorCmd",vectorCmd);

    if(std::popcount(u) != (argc-1)) {
        for(int i=1;i<argc;i++) {
            if(((static_cast<uint64_t>(1) << i) & u) == 0) {
                fprintf(logChannel,"found unhandled flag %s at position %d (1-based)\n",argv[i],i);
            }
        }
        exit(1);
    }
}