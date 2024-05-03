#pragma once
#include <cstdint>
#include <cstdlib>

void arenaInit();
size_t arenaSize();
void arenaReset(bool shrinkToFit);
void arenaFree();
void *arenaMalloc(size_t sz);
void *arenaCalloc(size_t nmemb,size_t sz);
bool arenaBelongsForSureToArena(const void *ptr);
