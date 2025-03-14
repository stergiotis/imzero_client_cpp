#pragma once
#include <cstdint>
#include <imgui.h>
#include <implot.h>
#include <cstdio>
#include <type_traits>
#include <string>
extern FILE *fdOut;
extern size_t totalSentBytes;

void sendInit();
void resetSendStat();

size_t fwrite_sendStat(const void *__restrict __ptr, size_t __size,
                      size_t __n, FILE *__restrict __s) noexcept;
inline size_t fwrite_sendStat(const void *__restrict __ptr, size_t __size,
                              size_t __n, FILE *__restrict __s) noexcept {
    const auto t = fwrite(__ptr,__size,__n,__s);
    totalSentBytes += t;
    return t;
}


template <typename T>
void sendValue(T v) {
    fwrite_sendStat(&v,sizeof(v),1,fdOut);
}
template <typename T>
void sendValueSignMagnitude(T v) {
    auto mask = static_cast<typename std::make_unsigned<T>::type>(1) << (sizeof(T)*8-1);
    auto u = static_cast<typename std::make_unsigned<T>::type>((v < 0) ? ((mask) | static_cast<typename std::make_unsigned<T>::type>(-v)) : v);
    fwrite_sendStat(&u,sizeof(u),1,fdOut);
}
template <typename T,int n>
void sendArray(const T *v) {
    fwrite_sendStat(v,sizeof(T),n,fdOut);
}
template <typename T>
void sendSlice(const T *v, size_t len) {
    sendValue<uint32_t>(static_cast<uint32_t>(len));
    if(len > 0){
        fwrite_sendStat(v,sizeof(T),len,fdOut);
    }
}
template <typename T>
void sendSlice(const T *begin, const T *endIncl) {
    const auto len = reinterpret_cast<uintptr_t>(endIncl)-reinterpret_cast<uintptr_t>(begin)+1;
    sendSlice<T>(begin,len);
}
void sendEmptyString();
void sendString(const char *v);
void sendString(const std::string &v);
void flushSend();

/* framework specific overloads*/

template <typename T,int n>
void sendArray(const ImVec2 &v) {
    sendValue<float>(v.x);
    sendValue<float>(v.y);
}
template <typename T,int n>
void sendArray(const ImVec4 &v) {
    sendValue<float>(v.x);
    sendValue<float>(v.y);
    sendValue<float>(v.z);
    sendValue<float>(v.w);
}
template <typename T,int n>
void sendArray(const ImPlotPoint &v) {
    sendValue<double>(v.x);
    sendValue<double>(v.y);
}
