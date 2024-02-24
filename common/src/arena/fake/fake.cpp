#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../arena.h"

void **ptrs = nullptr;
int pos=0;
constexpr int nSlots = 4096;

void arenaInit() {
    if(ptrs == nullptr){
        ptrs = (void**)calloc(nSlots,sizeof(void*));
    }
}
void arenaReset(shrinkToFit bool) {
    for(int i=0;i<pos;i++) {
        free(ptrs[i]);
    }
    pos=0;
}
void arenaFree() {
    free(ptrs);
    ptrs = nullptr;
    pos=0;
}
void *arenaMalloc(size_t sz) {
    void *r = malloc(sz);
    ptrs[pos]=r;
    pos++;
    return r;
}
void *arenaCalloc(size_t nmemb,size_t sz) {
    void *r = calloc(nmemb,sz);
    ptrs[pos]=r;
    pos++;
    return r;
}
bool arenaBelongsForSureToArena(const void *ptr) {
    // we can't tell...
    return false;
}
