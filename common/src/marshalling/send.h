#pragma once
#include <stdint.h>
#include "casts.h"
#include <imgui.h>
#include <implot.h>
#include <stdio.h>
#include <string.h>
#include <type_traits>
#include <string>
extern FILE *fdOut;

void sendInit();

template <typename T>
void sendValue(T v) {
    fwrite(&v,sizeof(v),1,fdOut);
}
template <typename T>
void sendValueSignMagnitude(T v) {
    auto mask = static_cast<typename std::make_unsigned<T>::type>(1) << (sizeof(T)*8-1);
    auto u = static_cast<typename std::make_unsigned<T>::type>((v < 0) ? ((mask) | static_cast<typename std::make_unsigned<T>::type>(-v)) : v);
    fwrite(&u,sizeof(u),1,fdOut);
}
template <typename T,int n>
void sendArray(const T *v) {
    fwrite(v,sizeof(T),n,fdOut);
}
template <typename T>
void sendSlice(const T *v, size_t len) {
    sendValue<uint32_t>((uint32_t)len);
    if(len > 0){
        fwrite(v,sizeof(T),len,fdOut);
    }
}
template <typename T>
void sendSlice(const T *begin, const T *endIncl) {
    auto len = (size_t)((uintptr_t)endIncl-(uintptr_t)begin+1);
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
