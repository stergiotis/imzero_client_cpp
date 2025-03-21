#include "send.h"
FILE *fdOut;
size_t totalSentBytes;

void sendInit() {
    resetSendStat();
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
