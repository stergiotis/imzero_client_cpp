#include <cstring>
#include <cassert>
#include "receive.h"
#include "helper.h"

#include "../arena/simple/simple.h"
static const char *findNull(const char *s) {
    return s+strlen(s);
}
const char ** convertNullSeparatedStringArrayToArray(char *nullsepstra, size_t &len) {
    return convertNullSeparatedStringArrayToArray(const_cast<const char *>(nullsepstra),len);
}
const char ** convertNullSeparatedStringArrayToArray(const char *nullsepstra, size_t &len) {
    len = findNullSeparatedStringArrayLength(nullsepstra);
    if(len == 0){
        return nullptr;
    }
    auto retr = static_cast<const char**>(arenaMalloc(len * sizeof(void*)));
    assert(retr != nullptr);

    retr[0] = nullsepstra;

    size_t l = getStringLength(nullsepstra);
    const char *end = nullsepstra+l+1;
    size_t n = 0;
    auto t=findNull(nullsepstra);
    while(t < end) {
        n++;
        retr[n] = t+1;
        t=findNull(t+1);
    }
    return retr;
}

size_t findNullSeparatedStringArrayLength(const char *nullsepstra) {
    const size_t l = getStringLength(nullsepstra);
    const char *end = nullsepstra+l+1;
    size_t n = 0;
    auto t=findNull(nullsepstra);
    while(t < end) {
        n++;
        t=findNull(t+1);
    }
    return n;
}
