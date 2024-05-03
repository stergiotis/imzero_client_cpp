#include "receive.h"
FILE *fdIn;

void receiveInit() {
    fdIn = stdin;
}
const char *receiveString() {
    auto l = receiveValue<uint32_t>();
    // use calloc if we receiveParam short, can be lowered to malloc as soon we handle fread errors
    auto r = static_cast<uint8_t *>(arenaCalloc(l+sizeof(l)+1,1));
    // store length adjacent to slice
    memcpy(r,&l,sizeof(l));
    fread(r+sizeof(l),1,l,fdIn);
//    r[sizeof(l)+l] = '\0';
    return (const char *)(r+sizeof(l));
}
const char *const *receiveStrings() {
    auto l = receiveValue<uint32_t>();
    // use calloc if we receiveParam short, can be lowered to malloc as soon we handle fread errors
    auto r = static_cast<uint8_t *>(arenaCalloc(l+sizeof(l),sizeof(const char*)));
    // store length adjacent to slice
    memcpy(r,&l,sizeof(l));
    for(uint32_t i=0;i<l;i++) {
	auto t = receiveString();
	memcpy(r+sizeof(l)+i*sizeof(const char*),&t,sizeof(const char*));
    }
    return (const char *const *)(r+sizeof(l));
}
uint8_t *receiveBytes() {
    return receiveSlice<uint8_t>();
}
size_t getParamStringLength(const char *str) {
    if(arenaBelongsForSureToArena(str)) {
        // read length stored _before_ the string
        uint32_t r;
        memcpy(&r,str-sizeof(r),sizeof(r));
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
