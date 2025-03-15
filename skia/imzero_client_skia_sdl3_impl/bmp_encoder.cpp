#include <bit>
#include <cstring>
#include "bmp_encoder.h"

static int writeUint32LE(void *out, uint32_t val) {
    if constexpr (std::endian::native == std::endian::big) {
        auto *out8 = static_cast<uint8_t*>(out);
        out8[0] = (val >> 24) & 0xff;
        out8[1] = (val >> 16) & 0xff;
        out8[2] = (val >> 8) & 0xff;
        out8[3] = (val & 0xff);
    } else if constexpr (std::endian::native == std::endian::little) {
        memcpy(out,&val,4);
    }
    static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);
    return 4;
}
static int writeUint16LE(void *out, uint16_t val) {
    if constexpr (std::endian::native == std::endian::big) {
        auto *out8 = static_cast<uint8_t*>(out);
        out8[0] = (val >> 8) & 0xff;
        out8[1] = (val & 0xff);
    } else if constexpr (std::endian::native == std::endian::little) {
        memcpy(out,&val,2);
    }
    static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);
    return 2;
}
static int writeUint8(void *out, uint8_t val) {
    auto *out8 = static_cast<uint8_t*>(out);
    out8[0] = val;
    return 1;
}

ImZeroClient::BmpBGRA8888Encoder::BmpBGRA8888Encoder(const uint32_t width, uint32_t height) {
    constexpr uint32_t colorSpaceWin = 0x206e6957;
    constexpr uint32_t colorSpaceSRGB = 0x42475273;
    constexpr uint32_t BmpHeaderSize = 2+4+2+2+4;
    constexpr uint32_t DibHeaderSize = 124;
    constexpr uint32_t bitsPerPixel = 32;
    //const uint32_t fRawSizeInclPadding = 4*((bitsPerPixel*desc.width+31)/32)*desc.height;
    fRawSizeInclPadding = width * height * 4; // special case 32 bit depth is always correctly padded

    fHeaderSize = BmpHeaderSize+DibHeaderSize;
    fHeader = static_cast<uint8_t*>(malloc(BmpHeaderSize+DibHeaderSize));
    auto *out8 = fHeader;

    /* BMP header */
    out8 += writeUint8(out8,'B');
    out8 += writeUint8(out8,'M');
    out8 += writeUint32LE(out8, BmpHeaderSize + DibHeaderSize + fRawSizeInclPadding);
    out8 += writeUint16LE(out8,0x0000); // Unused
    out8 += writeUint16LE(out8,0x0000); // Unused
    out8 += writeUint32LE(out8, BmpHeaderSize+DibHeaderSize); // Offet of pixel array

    /* DIB header */
    out8 += writeUint32LE(out8, DibHeaderSize);
    out8 += writeUint32LE(out8, width);
    height = ~height + 1; // calc two's complement to represent negative height in order to flip y axis
    out8 += writeUint32LE(out8, height);
    //out8 += writeUint32LE(out8, height);
    out8 += writeUint16LE(out8,1);  // Planes
    out8 += writeUint16LE(out8,bitsPerPixel); // Bits per pixel
    out8 += writeUint32LE(out8,0x03); // Format
    out8 += writeUint32LE(out8, fRawSizeInclPadding); // Image Raw Size Including Padding
    out8 += writeUint32LE(out8, 2835); // Horizontal print resolution (2835 = 72dpi * 39.3701 inch/m)
    out8 += writeUint32LE(out8, 2835); // Vertical print resolution (2835 = 72dpi * 39.3701 inch/m)
    out8 += writeUint32LE(out8, 0); // Colors in palette
    out8 += writeUint32LE(out8, 0); // Important colors (0 = all)
    //out8 += writeUint32LE(out8, 0x0000ff00); // R bitmask
    //out8 += writeUint32LE(out8, 0x00ff0000); // G bitmask
    //out8 += writeUint32LE(out8, 0xff000000); // B bitmask
    //out8 += writeUint32LE(out8, 0x000000ff); // A bitmask

    out8 += writeUint32LE(out8, 0x00ff0000); // R bitmask
    out8 += writeUint32LE(out8, 0x0000ff00); // G bitmask
    out8 += writeUint32LE(out8, 0x000000ff); // B bitmask
    out8 += writeUint32LE(out8, 0xff000000); // A bitmask
    out8 += writeUint32LE(out8, colorSpaceSRGB); // Colorspace
    memset(out8,0,9*4); out8 += 9*4; // CIEXYZTRIPLE Color Space endpoints
    memset(out8,0,3*4); out8 += 3*4; // Red,Green,Blue Gamma (unused)
    memset(out8,0,4*4); out8 += 4*4; // Unknown
}

ImZeroClient::BmpBGRA8888Encoder::~BmpBGRA8888Encoder() {
    if(fHeader != nullptr) {
        free(fHeader);
    }
}

size_t ImZeroClient::BmpBGRA8888Encoder::bmpTotalSize() const {
    return fRawSizeInclPadding+fHeaderSize;
}

void ImZeroClient::BmpBGRA8888Encoder::encode(void *out, const uint32_t *rgbaPixelsTLtoBR) const {
    memcpy(out,fHeader,fHeaderSize);
    memcpy(out, rgbaPixelsTLtoBR, fRawSizeInclPadding);
}

bool ImZeroClient::BmpBGRA8888Encoder::encode(SkWStream *stream, const uint32_t *rgbaPixelsTLtoBR) const {
    if(!stream->write(fHeader,fHeaderSize)) {
        return false;
    }
    if(!stream->write(rgbaPixelsTLtoBR, fRawSizeInclPadding)) {
        return false;
    }
    stream->flush();
    return true;
}
