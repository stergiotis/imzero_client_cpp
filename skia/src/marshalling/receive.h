#pragma once
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <cstddef>
#include <cassert>
#include "../arena/simple/simple.h"
extern FILE *fdIn;
extern size_t totalReceivedBytes;
constexpr const unsigned long alignment = alignof(std::max_align_t);
constexpr const unsigned long prefixedSizeOffset = (sizeof(uint32_t) > alignment) ? ((sizeof(uint32_t)/alignment)+1)*alignment : alignment;

void receiveInit();
size_t fread_receiveStat(void *__restrict ptr, size_t size, size_t n, FILE *__restrict stream) noexcept;
void resetReceiveStat();

template <typename T>
static T* checkAlignment(T *ptr) {
    assert(reinterpret_cast<uintptr_t>(ptr) % alignment == 0 && "pointer is not alignment");
    return ptr;
}

template <typename T>
static T receiveValue() {
    T r;
    fread_receiveStat(&r,sizeof(r),1,fdIn);
    return r;
}
template <typename T>
static T receiveValueSignMagnitude() {
    typename std::make_unsigned<T>::type mask = static_cast<typename std::make_unsigned<T>::type>(1) << (sizeof(T)*8-1);
    typename std::make_unsigned<T>::type u;
    fread_receiveStat(&u,sizeof(u),1,fdIn);
    return (u & mask) ? -static_cast<T>(u & ~mask) : u;
}
template <typename T,int n>
static T *receiveArray() {
    auto r = static_cast<T*>(arenaCalloc(n, sizeof(T)));
    fread_receiveStat(r,sizeof(T),n,fdIn);
    return r;
}
template <typename T>
static T *receiveSlice() {
    const auto l = receiveValue<uint32_t>();
    if(l == 0xffffffff) {
        // received nil slice
        return nullptr;
    }
    // use calloc if we receiveParam short, can be lowered to malloc as soon we handle fread errors
    auto *r = static_cast<uint8_t*>(arenaCalloc(l * sizeof(T) + prefixedSizeOffset, 1));
    // store length adjacent to slice
    memcpy(r,&l,sizeof(l));
    fread_receiveStat(r+prefixedSizeOffset,sizeof(T),l,fdIn);
    return reinterpret_cast<T*>(checkAlignment(r+prefixedSizeOffset));
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
        memcpy(&r,reinterpret_cast<const uint8_t*>(slice)-prefixedSizeOffset,sizeof(r));
        return r;
    } else {
        return -1;
    }
}
