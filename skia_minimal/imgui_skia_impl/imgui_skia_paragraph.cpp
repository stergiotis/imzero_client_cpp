#include <cassert>
#include "imgui_skia_paragraph.h"

Paragraph::Paragraph(sk_sp<SkFontMgr> fontMgr,sk_sp<SkTypeface> defaultTypeface) {
    fTlFontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    skia::textlayout::ParagraphStyle paraStyleLtr;
    skia::textlayout::ParagraphStyle paraStyleRtl;
    paraStyleLtr.setEllipsis(SkString("…"));
    //fTlParaStyle.setMaxLines(std::numeric_limits<size_t>::max()); // FIXME
    paraStyleLtr.setMaxLines(1000); // FIXME
    paraStyleLtr.setTextStyle(fTlTextStyle);

    paraStyleRtl.setMaxLines(paraStyleLtr.getMaxLines());
    paraStyleLtr.setTextStyle(fTlTextStyle);

    fTlFontCollection->setDefaultFontManager(fontMgr);
    fTlFontCollection->disableFontFallback();

    paraStyleLtr.setTextDirection(skia::textlayout::TextDirection::kLtr);
    paraStyleLtr.setTextAlign(skia::textlayout::TextAlign::kLeft);
    fParaBuilderLeftLTR = skia::textlayout::ParagraphBuilder::make(paraStyleLtr, fTlFontCollection);
    paraStyleLtr.setTextAlign(skia::textlayout::TextAlign::kRight);
    fParaBuilderRightLTR = skia::textlayout::ParagraphBuilder::make(paraStyleLtr, fTlFontCollection);
    paraStyleLtr.setTextAlign(skia::textlayout::TextAlign::kCenter);
    fParaBuilderCenterLTR = skia::textlayout::ParagraphBuilder::make(paraStyleLtr, fTlFontCollection);
    paraStyleLtr.setTextAlign(skia::textlayout::TextAlign::kJustify);
    fParaBuilderJustifyLTR = skia::textlayout::ParagraphBuilder::make(paraStyleLtr, fTlFontCollection);

    paraStyleRtl.setTextDirection(skia::textlayout::TextDirection::kRtl);
    paraStyleRtl.setTextAlign(skia::textlayout::TextAlign::kLeft);
    fParaBuilderLeftRTL = skia::textlayout::ParagraphBuilder::make(paraStyleRtl, fTlFontCollection);
    paraStyleRtl.setTextAlign(skia::textlayout::TextAlign::kRight);
    fParaBuilderRightRTL = skia::textlayout::ParagraphBuilder::make(paraStyleRtl, fTlFontCollection);
    paraStyleRtl.setTextAlign(skia::textlayout::TextAlign::kCenter);
    fParaBuilderCenterRTL = skia::textlayout::ParagraphBuilder::make(paraStyleRtl, fTlFontCollection);
    paraStyleRtl.setTextAlign(skia::textlayout::TextAlign::kJustify);
    fParaBuilderJustifyRTL = skia::textlayout::ParagraphBuilder::make(paraStyleRtl, fTlFontCollection);

    fParaBuilder = fParaBuilderLeftLTR.get();
    fTlTextStyle.setTypeface(defaultTypeface);

    fDefaultTypeface = defaultTypeface;
}
Paragraph::~Paragraph() = default;
sk_sp<SkTypeface> Paragraph::getDefaultTypeface() {
    return fDefaultTypeface;
    //return fFontMgr->matchFamilyStyle(nullptr,SkFontStyle()); // FIXME returns empty typeface
}
void Paragraph::getCacheStatistics(int &count) const {
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
SkScalar Paragraph::getMaxIntrinsicWidth() {
    return fPara->getMaxIntrinsicWidth();
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
SkRect Paragraph::boundingRect(int lineNumber, bool &found) {
    skia::textlayout::LineMetrics metrics;
    if(!fPara->getLineMetricsAt(lineNumber,&metrics)) {
        found = false;
        return SkRect::MakeEmpty();
    }
    found = true;
    return SkRect::MakeXYWH(static_cast<float>(metrics.fLeft),static_cast<float>(metrics.fBaseline-metrics.fAscent),
                            static_cast<float>(metrics.fLeft+metrics.fWidth),static_cast<float>(metrics.fBaseline+metrics.fDescent));
}
int Paragraph::getPath(int lineNumber, SkPath &dest) {
    return fPara->getPath(lineNumber,&dest);
}

void Paragraph::setForegroundPaint(const SkPaint &paint) {
    fTlTextStyle.setForegroundPaint(paint);
}
void Paragraph::paint(SkCanvas &canvas, SkScalar x, SkScalar y) {
    fPara->paint(&canvas,x,y);
}

void Paragraph::setFontSize(SkScalar size) {
    fTlTextStyle.setFontSize(size);
}

void Paragraph::setLetterSpacing(SkScalar sp) {
    fTlTextStyle.setLetterSpacing(sp);
}
void Paragraph::setTextLayout(skia::textlayout::TextAlign align,skia::textlayout::TextDirection dir) {
    switch(dir) {
        case skia::textlayout::TextDirection::kLtr:
            switch(align) {
                case skia::textlayout::TextAlign::kLeft: fParaBuilder = fParaBuilderLeftLTR.get(); break;
                case skia::textlayout::TextAlign::kRight: fParaBuilder = fParaBuilderRightLTR.get(); break;
                case skia::textlayout::TextAlign::kCenter: fParaBuilder = fParaBuilderCenterLTR.get(); break;
                case skia::textlayout::TextAlign::kJustify: fParaBuilder = fParaBuilderJustifyLTR.get(); break;
                default:
                    assert("unhandled align enumeration value");
            }
            break;
        case skia::textlayout::TextDirection::kRtl:
            switch(align) {
                case skia::textlayout::TextAlign::kLeft: fParaBuilder = fParaBuilderLeftRTL.get(); break;
                case skia::textlayout::TextAlign::kRight: fParaBuilder = fParaBuilderRightRTL.get(); break;
                case skia::textlayout::TextAlign::kCenter: fParaBuilder = fParaBuilderCenterRTL.get(); break;
                case skia::textlayout::TextAlign::kJustify: fParaBuilder = fParaBuilderJustifyRTL.get(); break;
                default:
                    assert("unhandled align enumeration value");
            }
            break;
    }
}

int Paragraph::getNumberOfLines() {
    int m = 0;
    fPara->visit([&m](int lineNumber,const skia::textlayout::Paragraph::VisitorInfo *info){
       m = std::max(m, lineNumber);
    });
    return m;
}
bool Paragraph::hasLine(int lineNumber) {
    return fPara->getLineMetricsAt(lineNumber, nullptr);
}

void Paragraph::enableFontFallback() {
    fTlFontCollection->enableFontFallback();
}

void Paragraph::disableFontFallback() {
    fTlFontCollection->disableFontFallback();
}

#if 0
void Paragraph::triangulate(int lineNumber,const SkRect &clipBounds,const float *&vertices,size_t &numVertices, int &unrenderedGlyphs) {
    if(!hasLine(lineNumber)) {
        return;
    }
    SkPath p;
    unrenderedGlyphs = getPath(lineNumber,p);
    auto const tol = GrPathUtils::scaleToleranceToSrc(GrPathUtils::kDefaultTolerance, SkMatrix::I(), p.getBounds());
    bool isLinear;

    int count = GrTriangulator::PathToTriangles(p, tol, clipBounds, &vertexAllocator, &isLinear);
    if (count > 0) {
        auto const data = vertexAllocator.detachVertexData();
        assert(data->vertexSize() == 2*sizeof(float));
        vertices = static_cast<const float*>(data->vertices());
        numVertices = data->numVertices();
    } else {
        vertices = nullptr;
        numVertices = 0;
    }
}
#endif