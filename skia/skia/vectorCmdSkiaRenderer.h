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
#include "vectorCmd_generated.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "paragraph.h"
#include <memory>

#include "vectorCmdSkiRendererConfig.h"

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
        void drawVectorCmdsFBDrawList(const VectorCmdFB::DrawList *drawListFb, SkCanvas &canvas, bool inner);
        void drawVectorCmdFB(const VectorCmdFB::SingleVectorCmdDto *cmdUnion, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);

        void drawCmdPolylineFB(const VectorCmdFB::CmdPolyline &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdConvexPolylineFilledFB(const VectorCmdFB::CmdConvexPolyFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdNgonFB(const VectorCmdFB::CmdNgon &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdNgonFilledFB(const VectorCmdFB::CmdNgonFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdLineFB(const VectorCmdFB::CmdLine &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdRectRoundedFB(const VectorCmdFB::CmdRectRounded &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdRectRoundedFilledFB(const VectorCmdFB::CmdRectRoundedFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdRectRoundedCornersFB(const VectorCmdFB::CmdRectRoundedCorners &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdRectRoundedCornersFilledFB(const VectorCmdFB::CmdRectRoundedCornersFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdRectFilledMultiColorFB(const VectorCmdFB::CmdRectFilledMultiColor &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdQuadFB(const VectorCmdFB::CmdQuad &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdQuadFilledFB(const VectorCmdFB::CmdQuadFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdTriangleFB(const VectorCmdFB::CmdTriangle &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdTriangleFilledFB(const VectorCmdFB::CmdTriangleFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdRenderTextFB(const VectorCmdFB::CmdRenderText &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdRenderParagraphFB(const VectorCmdFB::CmdRenderParagraph &cmd, SkCanvas &canvas, VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdCircleFB(const VectorCmdFB::CmdCircle &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdCircleFilledFB(const VectorCmdFB::CmdCircleFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void handleCmdPushClipRect(const VectorCmdFB::CmdPushClipRect &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void handleCmdPopClipRect(const VectorCmdFB::CmdPopClipRect &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdEllipseFB(const VectorCmdFB::CmdEllipse &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdEllipseFilledFB(const VectorCmdFB::CmdEllipseFilled &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdBezierCubicFB(const VectorCmdFB::CmdBezierCubic &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdBezierQuadraticFB(const VectorCmdFB::CmdBezierQuadratic &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void drawCmdVertexDraw(const VectorCmdFB::CmdVertexDraw &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);
        void registerFont(const VectorCmdFB::CmdRegisterFont &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags);

        RenderModeE getRenderMode() const;
        void changeRenderMode(RenderModeE r);
        void setVertexDrawPaint(SkPaint *vertexPaintP);
        void setParagraphHandler(std::shared_ptr<Paragraph> paragraph);

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
        void loadVertices(const VectorCmdFB::VertexData *vertices);

        std::shared_ptr<Paragraph> fParagraph;
        SkFont fFont;

        RenderModeE fRenderMode;

        void prepareFillPaint(SkPaint &paint,VectorCmdFB::DrawListFlags dlFlags) const;
        void prepareOutlinePaint(SkPaint &paint,VectorCmdFB::DrawListFlags dlFlags) const;

#ifdef RENDER_MODE_BACKDROP_FILTER_ENABLED
        sk_sp<SkImageFilter> backdropFilter;
        void setupWindowRectPaint(SkPaint &paint, VectorCmdFB::DrawListFlags dlFlags, uint32_t col);
#endif
        void drawRectRounded(const SkRect &rect, float r,SkCanvas &canvas,SkPaint &paint);
        void drawRRect(const SkRRect &rect, SkCanvas &canvas,SkPaint &paint);

};