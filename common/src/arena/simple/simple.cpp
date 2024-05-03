#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <cstddef>

uint8_t *arena = nullptr;
int64_t arenaPos = 0;
#ifdef __EMSCRIPTEN__
constexpr int64_t arenaAllocSize = UINT32_MAX;
#else
constexpr int64_t arenaAllocSize = 8ULL*1024ULL*1024ULL*1024ULL;
#endif

void arenaInit() {
    if(arena == nullptr) {
        arena = (uint8_t*)malloc(arenaAllocSize);
        assert(arena != nullptr);
        arenaPos = 0;
    }
}
size_t arenaSize() {
    return arenaPos;
}

void arenaReset(bool shrinkToFit) {
    arenaPos = 0;
}
void arenaFree() {
    free(arena);
    arena = nullptr;
    arenaPos = 0;
}
void *arenaMalloc(size_t sz) {
    assert((int64_t)sz <= (arenaAllocSize-arenaPos));
    if((int64_t)sz > (arenaAllocSize-arenaPos)) {
	    fprintf(stderr,"sz=%ld,pos=%ld\n",(long)sz,(long)arenaPos);
    	return nullptr;
    }
    auto r = arena+arenaPos;
    arenaPos+=sz;
    // ensure the _next_ allocation will be properly aligned
    // assume that the modulo operations gets narrowed to a bitwise and
    arenaPos+=arenaPos % alignof(std::max_align_t);
    return r;
}
void *arenaCalloc(size_t nmemb,size_t sz) {
    auto s = nmemb*sz;
    auto r = arenaMalloc(s);
    assert(r != nullptr);
    memset(r,0x00,s);
    return r;
}
bool arenaBelongsForSureToArena(const void *ptr) {
    return ((uintptr_t)ptr >= (uintptr_t)arena && (uintptr_t)ptr < (uintptr_t)arena + (uintptr_t)arenaAllocSize);
}
