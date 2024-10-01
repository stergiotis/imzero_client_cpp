#pragma once
#include <cstdint>
#include <cstdlib>
#include "include/core/SkStream.h"

class BmpBGRA8888Encoder {
public:
    BmpBGRA8888Encoder() = delete;
    BmpBGRA8888Encoder(uint32_t width, uint32_t height);
    ~BmpBGRA8888Encoder();

    size_t bmpTotalSize() const;
    void encode(void *out, const uint32_t *rgbaPixelsTLtoBR) const;
    bool encode(SkWStream *stream, const uint32_t *rgbaPixelsTLtoBR) const;

private:
    uint8_t *fHeader = nullptr;
    size_t fHeaderSize = 0;
    size_t fRawSizeInclPadding = 0;
};
