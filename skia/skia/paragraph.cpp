#include "paragraph.h"
#include <cassert>

Paragraph::Paragraph(sk_sp<SkFontMgr> fontMgr,sk_sp<SkTypeface> defaultTypeface) {
    fTlFontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    fTlParaStyle.setEllipsis(SkString("…"));
    //fTlParaStyle.setMaxLines(std::numeric_limits<size_t>::max()); // FIXME
    fTlParaStyle.setMaxLines(1000); // FIXME
    fTlParaStyle.setTextStyle(fTlTextStyle);
    fTlFontCollection->setDefaultFontManager(fontMgr);

    fParaBuilder = skia::textlayout::ParagraphBuilderImpl::make(fTlParaStyle, fTlFontCollection);
    fFontMgr = fontMgr;
    fDefaultTypeface = defaultTypeface;
}
Paragraph::~Paragraph() = default;
sk_sp<SkTypeface> Paragraph::getDefaultTypeface() {
    return fDefaultTypeface;
    //return fFontMgr->matchFamilyStyle(nullptr,SkFontStyle()); // FIXME returns empty typeface
}
void Paragraph::getCacheStatistics(int &count) {
    const auto cache = fTlFontCollection->getParagraphCache();
    if(cache == nullptr) {
        count = 0;
        return;
    }
    //cache->printStatistics();
    count = cache->count();
    // TODO: there are more fields in ParagraphCache, but these are private...
}
void Paragraph::setCacheEnable(bool enable) {
    auto cache = fTlFontCollection->getParagraphCache();
    if(cache == nullptr) {
        return;
    }
    cache->turnOn(enable);
}
void Paragraph::resetCache() {
    const auto cache = fTlFontCollection->getParagraphCache();
    if(cache == nullptr) {
        return;
    }
    cache->reset();
}
SkScalar Paragraph::getMaxWidth() {
    return fPara->getMaxWidth();
}
SkScalar Paragraph::getHeight() {
    return fPara->getHeight();
}
void Paragraph::build(const char *text,size_t len) {
    fParaBuilder->Reset();
    fParaBuilder->pushStyle(fTlTextStyle);
    fParaBuilder->addText(text,len);
    fPara = fParaBuilder->Build();
}
void Paragraph::layout(SkScalar width) {
    fPara->layout(width);
}
void Paragraph::setForegroundPaint(SkPaint &paint) {
    fTlTextStyle.setForegroundPaint(paint);
}
void Paragraph::paint(SkCanvas &canvas, SkScalar x, SkScalar y) {
    fPara->paint(&canvas,x,y);
}