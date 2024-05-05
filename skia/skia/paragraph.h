#pragma once

#include "modules/skparagraph/include/Paragraph.h"
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "core/SkFontMgr.h"
#include "core/SkTypeface.h"

class Paragraph {
    public:
        Paragraph(sk_sp<SkFontMgr> fontMgr, sk_sp<SkTypeface> defaultTypeface);
        ~Paragraph();

        SkScalar getMaxWidth();
        SkScalar getHeight();
        void build(const char *text,size_t len);
        void layout(SkScalar width);
        void paint(SkCanvas &canvas, SkScalar x, SkScalar y);
        void setForegroundPaint(SkPaint &paint);

        void getCacheStatistics(int &count);
        void setCacheEnable(bool enable);
        void resetCache();

        sk_sp<SkTypeface> getDefaultTypeface();

    private:
        sk_sp<skia::textlayout::FontCollection> fTlFontCollection;
        sk_sp<SkFontMgr> fFontMgr;
        sk_sp<SkTypeface> fDefaultTypeface;
        skia::textlayout::TextStyle fTlTextStyle;
        skia::textlayout::ParagraphStyle fTlParaStyle;
        std::unique_ptr<skia::textlayout::ParagraphBuilder> fParaBuilder;
        std::unique_ptr<skia::textlayout::Paragraph> fPara;
};