#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <bit>
#include <cmath>
#include "cliOptions.h"
#include "buildinfo.gen.h"

static const char *findFlagValueDefault(FILE *logChannel, uint64_t &markUsed, int argc, char **argv,const char *flag,const char *defaultValue) {
    for(int i=1;i<argc;i++) {
        auto t = uint64_t(1) << (i);
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
    fprintf(file, "   version: git-commit=\"%s\",dirty=\"%s\"\n\n",buildinfo::gitCommit,buildinfo::gitDirty ? "yes" : "no");
    fprintf(file, "   -help\n");
    fprintf(file," string flags:\n");
    fprintf(file,"    -ttfFilePath [path:%s]\n",ttfFilePath);
    fprintf(file,"    -fffiInFile [path:%s]\n",fffiInFile == nullptr ? "<stdin>" : fffiInFile);
    fprintf(file,"    -fffiOutFile [path:%s]\n",fffiOutFile == nullptr ? "<stdout>" : fffiOutFile);
    fprintf(file,"    -appTitle [title:%s]\n", appTitle);
    fprintf(file,"    -skiaBackendType [type:%s]    choices: raster,gl,vulkan\n",skiaBackendType);
    fprintf(file," float flags:\n");
    fprintf(file,"    -fontDyFudge [float:%f]\n", fontDyFudge);
    fprintf(file," bool flags:\n");
    fprintf(file,"    -fffiInterpreter [bool:%s]\n",fffiInterpreter ? "on" : "off");
    fprintf(file,"    -vsync [bool:%s]\n",vsync ? "on" : "off");
    fprintf(file, "    -backdropFilter [bool:%s]\n", backdropFilter ? "on" : "off");
    fprintf(file, "    -sketchFilter [bool:%s]\n", sketchFilter ? "on" : "off");
}
void CliOptions::parse(int argc,char **argv,FILE *logChannel) {
    if(argc > 1) {
        if(strcmp(argv[1],"-help") == 0) {
            usage(argv[0],stderr);
            exit(0);
        }
    }
    auto u = uint64_t(0);

    ttfFilePath = findFlagValueDefault(logChannel,u, argc, argv,"-ttfFilePath",defaultTtfFilePath);
    fffiInFile = findFlagValueDefault(logChannel,u, argc, argv, "-fffiInFile", fffiInFile);
    fffiOutFile = findFlagValueDefault(logChannel,u, argc, argv, "-fffiOutFile", fffiOutFile);
    appTitle = findFlagValueDefault(logChannel,u, argc, argv, "-appTitle", appTitle);
    skiaBackendType = findFlagValueDefault(logChannel,u, argc, argv, "-skiaBackendType", skiaBackendType);

    fontDyFudge = findFlagValueDefaultFloat(logChannel,u, argc, argv, "-fontDyFudge", "0.0");
    if(std::isnan(fontDyFudge) || fontDyFudge < -10000.0f || fontDyFudge > 10000.0f) {
        fprintf(logChannel,"implausible value for -fontDyFudge: %f\n", fontDyFudge);
        exit(1);
    }

    fffiInterpreter = getBoolFlagValue(logChannel,u,argc,argv,"-fffiInterpreter",fffiInterpreter);
    vsync = getBoolFlagValue(logChannel,u,argc,argv,"-vsync",vsync);
    backdropFilter = getBoolFlagValue(logChannel,u, argc, argv, "-backdropFilter",backdropFilter);
    sketchFilter = getBoolFlagValue(logChannel,u, argc, argv, "-sketchFilter",sketchFilter);

    if(std::popcount(u) != (argc-1)) {
        for(int i=1;i<argc;i++) {
            if(((uint64_t(1) << i) & u) == 0) {
                fprintf(logChannel,"found unhandled flag %s at position %d (1-based)\n",argv[i],i);
            }
        }
        exit(1);
    }
}