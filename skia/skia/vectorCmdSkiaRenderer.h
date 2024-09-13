#pragma once
#include "include/core/SkBlendMode.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkSamplingOptions.h"
#include "include/core/SkShader.h"
#include "include/core/SkSurface.h"
#include "include/core/SkSwizzle.h"
#include "include/core/SkVertices.h"
#include "src/core/SkClipStack.h"
#include "include/private/base/SkTDArray.h"
#include "src/base/SkTime.h"
#include "tools/skui/InputState.h"
#include "tools/skui/Key.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkPicture.h"
#include "include/core/SkStream.h"

#include "flatbuffers/flatbuffers.h"
#include "ImZeroFB.out.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "paragraph.h"
#include <memory>

#include "imzeroSkiaRenderConfig.h"

#include <cstdint>

typedef int RenderModeE;
enum RenderModeE_ {
    RenderModeE_Normal = 0 << 0,
    RenderModeE_Sketch = 1 << 1,
    RenderModeE_SVG = 1 << 2,
    RenderModeE_BackdropBlur = 1 << 3,
};

class VectorCmdSkiaRenderer {
    public:
        VectorCmdSkiaRenderer();
        ~VectorCmdSkiaRenderer();

        void prepareForDrawing();

        void drawSerializedVectorCmdsFB(const uint8_t *buf, SkCanvas &canvas);
        void drawVectorCmdsFBDrawList(const ImZeroFB::DrawList *drawListFb, SkCanvas &canvas, bool inner);
        void drawVectorCmdFB(const ImZeroFB::SingleVectorCmdDto *cmdUnion, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);

        void drawCmdPolylineFB(const ImZeroFB::CmdPolyline &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdConvexPolylineFilledFB(const ImZeroFB::CmdConvexPolyFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdConcavePolylineFilledFB(const ImZeroFB::CmdConcavePolyFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdNgonFB(const ImZeroFB::CmdNgon &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdNgonFilledFB(const ImZeroFB::CmdNgonFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdLineFB(const ImZeroFB::CmdLine &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdRectRoundedFB(const ImZeroFB::CmdRectRounded &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdRectRoundedFilledFB(const ImZeroFB::CmdRectRoundedFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdRectRoundedCornersFB(const ImZeroFB::CmdRectRoundedCorners &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdRectRoundedCornersFilledFB(const ImZeroFB::CmdRectRoundedCornersFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdRectFilledMultiColorFB(const ImZeroFB::CmdRectFilledMultiColor &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdQuadFB(const ImZeroFB::CmdQuad &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdQuadFilledFB(const ImZeroFB::CmdQuadFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdTriangleFB(const ImZeroFB::CmdTriangle &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdTriangleFilledFB(const ImZeroFB::CmdTriangleFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdRenderTextFB(const ImZeroFB::CmdRenderText &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdRenderParagraphFB(const ImZeroFB::CmdRenderParagraph &cmd, SkCanvas &canvas, ImZeroFB::DrawListFlags dlFlags);
        void drawCmdRenderUnicodeCodepointFB(const ImZeroFB::CmdRenderUnicodeCodepoint &cmd, SkCanvas &canvas, ImZeroFB::DrawListFlags dlFlags);
        void drawCmdCircleFB(const ImZeroFB::CmdCircle &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdCircleFilledFB(const ImZeroFB::CmdCircleFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void handleCmdPushClipRect(const ImZeroFB::CmdPushClipRect &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void handleCmdPopClipRect(const ImZeroFB::CmdPopClipRect &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdEllipseFB(const ImZeroFB::CmdEllipse &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdEllipseFilledFB(const ImZeroFB::CmdEllipseFilled &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdBezierCubicFB(const ImZeroFB::CmdBezierCubic &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdBezierQuadraticFB(const ImZeroFB::CmdBezierQuadratic &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdVertexDraw(const ImZeroFB::CmdVertexDraw &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdSimpleVertexDraw(const ImZeroFB::CmdSimpleVertexDraw &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdSvgPathSubset(const ImZeroFB::CmdSvgPathSubset &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void drawCmdPath(const ImZeroFB::CmdPath &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);
        void registerFont(const ImZeroFB::CmdRegisterFont &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags);

        RenderModeE getRenderMode() const;
        void changeRenderMode(RenderModeE r);
        void setVertexDrawPaint(SkPaint *vertexPaintP);
        void setParagraphHandler(std::shared_ptr<Paragraph> paragraph);
        sk_sp<SkTypeface> getTypeface() const;

    private:
        // stack of pre-intersected clipping rectangles
#ifdef SKIA_DRAW_BACKEND_DEBUG_CLIPPING
        SkTDArray<std::pair<SkRect,bool>> fClipStack;
#else
        SkTDArray<SkRect> fClipStack;
#endif

        SkPaint *fVertexPaint;
        SkTDArray<SkPoint> vtxXYs;
        SkTDArray<SkPoint> vtxTexUVs;
        SkTDArray<SkColor> vtxColors;
        SkTDArray<uint16_t> vtxIndices;
        void loadVertices(const ImZeroFB::VertexData *vertices);

        //SkTDArray<SkPoint> vtxXYSimples;
        std::shared_ptr<Paragraph> fParagraph;
        SkFont fFont;
        sk_sp<SkData> fFontData;
        sk_sp<SkTypeface> fTypeface;
        sk_sp<SkFontMgr> fFontMgr;

        RenderModeE fRenderMode;

        void prepareFillPaint(SkPaint &paint,ImZeroFB::DrawListFlags dlFlags) const;
        void prepareOutlinePaint(SkPaint &paint,ImZeroFB::DrawListFlags dlFlags) const;
        void prepareFillAndOutlinePaint(SkPaint &paint,ImZeroFB::DrawListFlags dlFlags) const;

#ifdef RENDER_MODE_BACKDROP_FILTER_ENABLED
        sk_sp<SkImageFilter> backdropFilter;
        void setupWindowRectPaint(SkPaint &paint, ImZeroFB::DrawListFlags dlFlags, uint32_t col);
#endif
        void drawRectRounded(const SkRect &rect, float r,SkCanvas &canvas,SkPaint &paint);
        void drawRRect(const SkRRect &rect, SkCanvas &canvas,SkPaint &paint);

};
