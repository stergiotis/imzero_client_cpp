#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstddef>
#include <memory>

uint8_t *arena = nullptr;
int64_t arenaPos = 0;
int64_t arenaAllocSize = 8ULL*1024ULL*1024ULL*1024ULL;
constexpr int64_t alignment = alignof(std::max_align_t);

static void adjustAlignment() {
    // ensure the _next_ allocation will be properly aligned
    arenaPos = (arenaPos - 1 + alignment) & -alignment;
}
void arenaInit() {
    if(arena == nullptr) {
        arena = static_cast<uint8_t*>(malloc(arenaAllocSize));
        assert(arena != nullptr);
        adjustAlignment();
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
    assert(static_cast<int64_t>(sz) <= (arenaAllocSize-arenaPos));
    if(static_cast<int64_t>(sz) > (arenaAllocSize-arenaPos)) {
	    fprintf(stderr,"sz=%ld,pos=%ld\n",static_cast<long>(sz),arenaPos);
    	return nullptr;
    }
    const auto r = arena+arenaPos;
    arenaPos+=static_cast<int64_t>(sz);
    adjustAlignment();
    return r;
}
void *arenaCalloc(size_t nmemb,size_t sz) {
    const auto s = nmemb*sz;
    const auto r = arenaMalloc(s);
    assert(r != nullptr);
    memset(r,0x00,s);
    return r;
}
bool arenaBelongsForSureToArena(const void *ptr) {
    return (reinterpret_cast<uintptr_t>(ptr) >= reinterpret_cast<uintptr_t>(arena) && reinterpret_cast<uintptr_t>(ptr) < reinterpret_cast<uintptr_t>(arena) + static_cast<uintptr_t>(arenaAllocSize));
}
