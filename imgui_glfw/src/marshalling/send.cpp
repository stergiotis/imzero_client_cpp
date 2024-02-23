#include "send.h"
FILE *fdOut;

void sendInit() {
    fdOut = stdout;
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
