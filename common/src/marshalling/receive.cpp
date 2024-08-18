#include "receive.h"
FILE *fdIn;
size_t totalReceivedBytes;

size_t fread_receiveStat(void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __stream) noexcept {
    auto t = fread(__ptr,__size,__n,__stream);
    totalReceivedBytes += t;
    return t;
}
void resetReceiveStat() {
    totalReceivedBytes = 0;
}
void receiveInit() {
    fdIn = stdin;
    resetReceiveStat();
}
const char *receiveString() {
    auto l = receiveValue<uint32_t>();
    // use calloc if we receiveParam short, can be lowered to malloc as soon we handle fread errors
    auto r = static_cast<uint8_t *>(arenaCalloc(l+1+prefixedSizeOffset,1));
    // store length adjacent to slice
    memcpy(r,&l,sizeof(l));
    fread_receiveStat(r+prefixedSizeOffset,1,l,fdIn);
//    r[sizeof(l)+l] = '\0';
    return (const char *)(r+prefixedSizeOffset);
}
const char *const *receiveStrings() {
    auto l = receiveValue<uint32_t>();
    // use calloc if we receiveParam short, can be lowered to malloc as soon we handle fread errors
    auto r = static_cast<uint8_t *>(arenaCalloc(l*sizeof(const char*)+prefixedSizeOffset,1));
    // store length adjacent to slice
    memcpy(r,&l,sizeof(l));
    for(uint32_t i=0;i<l;i++) {
        auto t = receiveString();
        memcpy(r+prefixedSizeOffset+i*sizeof(const char*),&t,sizeof(const char*));
    }
    return (const char *const *)(r+prefixedSizeOffset);
}
uint8_t *receiveBytes() {
    return receiveSlice<uint8_t>();
}
size_t getParamStringLength(const char *str) {
    if(arenaBelongsForSureToArena(str)) {
        // read length stored _before_ the string
        uint32_t r;
        memcpy(&r,str-prefixedSizeOffset,sizeof(r));
        return (size_t)(r);
    } else {
        return strlen(str);
    }
}
size_t getStringLength(const char *str) {
    auto sz = getSliceLength<char>(str);
    if(sz == (size_t)-1) {
        return strlen(str);
    }
    return sz;
}
