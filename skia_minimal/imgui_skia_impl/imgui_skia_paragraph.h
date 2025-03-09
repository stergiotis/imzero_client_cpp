#pragma once

#include "modules/skparagraph/include/Paragraph.h"
#include "src/gpu/ganesh/GrEagerVertexAllocator.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkTypeface.h"

#include "src/gpu/ganesh/geometry/GrPathUtils.h"
#include "src/gpu/ganesh/geometry/GrTriangulator.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"

class Paragraph {
    public:
        Paragraph(sk_sp<SkFontMgr> fontMgr, sk_sp<SkTypeface> defaultTypeface);
        ~Paragraph();

        SkScalar getMaxWidth();
        SkScalar getMaxIntrinsicWidth();
        SkScalar getHeight();
        void build(const char *text,size_t len);
        void layout(SkScalar width);
        int getPath(int lineNumber, SkPath &dest);
        int getNumberOfLines();
        bool hasLine(int lineNumber);

#if 0
        void triangulate(int lineNumber,const SkRect &clipBounds,const float *&vertices,size_t &numVertices,int &unrenderedGlyphs);
#endif
        SkRect boundingRect(int lineNumber, bool &found);

        void paint(SkCanvas &canvas, SkScalar x, SkScalar y);
        void setForegroundPaint(const SkPaint &paint);

        void getCacheStatistics(int &count) const;
        void setCacheEnable(bool enable);
        void resetCache();
        void setFontSize(SkScalar size);
        void setLetterSpacing(SkScalar sp);
        void setTextLayout(skia::textlayout::TextAlign align,skia::textlayout::TextDirection dir);
        void setMaxLines(int n);

        sk_sp<SkTypeface> getDefaultTypeface();
        void enableFontFallback();
        void disableFontFallback();

    private:
        sk_sp<skia::textlayout::FontCollection> fTlFontCollection;
        sk_sp<SkTypeface> fDefaultTypeface;
        skia::textlayout::TextStyle fTlTextStyle;
        skia::textlayout::ParagraphBuilder *fParaBuilder;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderLeftLTR;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderRightLTR;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderCenterLTR;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderJustifyLTR;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderLeftRTL;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderRightRTL;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderCenterRTL;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilderJustifyRTL;
        std::unique_ptr<skia::textlayout::Paragraph> fPara;
#if 0
        GrCpuVertexAllocator vertexAllocator;
#endif
};
