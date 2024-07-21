#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include "app.h"

static void handleErr(const char *msg) {
    if(msg != nullptr) {
        fprintf(stderr, "error: %s\n", msg);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    App app;
    if(argc != 2) {
        fprintf(stderr,"usage: %s <file>\n",argv[0]);
        return 1;
    }
    handleErr(app.setup(argv[1],stdout));
    bool quit = false;
    while(!quit) {
        handleErr(app.step(quit));
    }

    return 0;
}