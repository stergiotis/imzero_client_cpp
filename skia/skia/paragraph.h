#pragma once

#include "modules/skparagraph/include/Paragraph.h"
#include "src/gpu/ganesh/GrEagerVertexAllocator.h"
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "core/SkFontMgr.h"
#include "core/SkTypeface.h"

#include "src/gpu/ganesh/GrEagerVertexAllocator.h"
#include "src/gpu/ganesh/geometry/GrPathUtils.h"
#include "src/gpu/ganesh/geometry/GrTriangulator.h"
#include "core/SkSpan.h"

class Paragraph {
    public:
        Paragraph(sk_sp<SkFontMgr> fontMgr, sk_sp<SkTypeface> defaultTypeface);
        ~Paragraph();

        SkScalar getMaxWidth();
        SkScalar getHeight();
        void build(const char *text,size_t len);
        void layout(SkScalar width);
        int getPath(int lineNumber, SkPath &dest);
        int getNumberOfLines();
        bool hasLine(int lineNumber);
        void triangulate(int lineNumber,const SkRect &clipBounds,const float *&vertices,size_t &numVertices,int &unrenderedGlyphs);
        SkRect boundingRect(int lineNumber, bool &found);

        void paint(SkCanvas &canvas, SkScalar x, SkScalar y);
        void setForegroundPaint(SkPaint &paint);

        void getCacheStatistics(int &count);
        void setCacheEnable(bool enable);
        void resetCache();
        void setFontSize(SkScalar size);
        void setLetterSpacing(SkScalar sp);
        void setTextAlign(skia::textlayout::TextAlign align);

        sk_sp<SkTypeface> getDefaultTypeface();

    private:
        sk_sp<skia::textlayout::FontCollection> fTlFontCollection;
        sk_sp<SkFontMgr> fFontMgr;
        sk_sp<SkTypeface> fDefaultTypeface;
        skia::textlayout::TextStyle fTlTextStyle;
        skia::textlayout::ParagraphBuilder *fParaBuilder;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderLeft;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderRight;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderCenter;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderJustify;
        std::unique_ptr<skia::textlayout::Paragraph> fPara;
        GrCpuVertexAllocator vertexAllocator;
};