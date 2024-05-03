#include "send.h"
FILE *fdOut;
size_t totalSentBytes;

void sendInit() {
    fdOut = stdout;
    resetSendStat();
}
inline size_t fwrite_sendStat(const void *__restrict __ptr, size_t __size,
                              size_t __n, FILE *__restrict __s) noexcept {
    auto t = fwrite(__ptr,__size,__n,__s);
    totalSentBytes += t;
    return t;
}
void resetSendStat() {
    totalSentBytes = 0;
}

void sendEmptyString() {
    sendValue<uint32_t>(0);
}
void sendString(const std::string &v) {
    size_t len = v.length();
    if(len == 0) {
        sendEmptyString();
    } else {
        sendSlice<char>(v.data(),len);
    }
}
void sendString(const char *v) {
    size_t len = 0;
    if(v != nullptr) {
        len = strlen(v);
    }
    if(len == 0) {
        sendEmptyString();
    } else {
        sendSlice<char>(v,len);
    }
}

void flushSend() {
    fflush(fdOut);
}
