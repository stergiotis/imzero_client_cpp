#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <type_traits>
#include "../arena/simple/simple.h"
extern FILE *fdIn;

void receiveInit();

template <typename T>
static T receiveValue() {
    T r;
    fread(&r,sizeof(r),1,fdIn);
    return r;
}
template <typename T>
static T receiveValueSignMagnitude() {
    typename std::make_unsigned<T>::type mask = static_cast<typename std::make_unsigned<T>::type>(1) << (sizeof(T)*8-1);
    typename std::make_unsigned<T>::type u;
    fread(&u,sizeof(u),1,fdIn);
    return (u & mask) ? -(T)(u & ~mask) : u;
}
template <typename T,int n>
static T *receiveArray() {
    auto r = (T *)arenaCalloc(n,sizeof(T));
    fread(r,sizeof(T),n,fdIn);
    return r;
}
template <typename T>
static T *receiveSlice() {
    auto l = receiveValue<uint32_t>();
    if(l == 0xffffffff) {
        // received nil slice
        return nullptr;
    }
    // use calloc if we receiveParam short, can be lowered to malloc as soon we handle fread errors
    uint8_t *r = (uint8_t *)arenaCalloc(l+sizeof(l),sizeof(T));
    // store length adjacent to slice
    memcpy(r,&l,sizeof(l));
    fread(r+sizeof(l),sizeof(T),l,fdIn);
    return (T*)(r+sizeof(l));
}
const char *receiveString();
const char* const* receiveStrings();
uint8_t *receiveBytes();

size_t getStringLength(const char *str);
template <typename T>
size_t getSliceLength(const T *slice) {
    if(arenaBelongsForSureToArena(slice)) {
        // read length stored _before_ the slice
        uint32_t r;
        memcpy(&r,((uint8_t*)slice)-sizeof(r),sizeof(r));
        return r;
    } else {
        return -1;
    }
}
