#pragma once
#include <cstdio>

void render_init(FILE *fdInput, FILE *fdOutput);
void render_render();
void render_cleanup();
void interpretCommands();
