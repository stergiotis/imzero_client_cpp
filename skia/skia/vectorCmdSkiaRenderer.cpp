#include "vectorCmdSkiaRenderer.h"

#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRRect.h"
#include "include/core/SkScalar.h"
#include "include/core/SkTypes.h"
#include "include/core/SkSwizzle.h"
#include "include/private/base/SkTArray.h"
#include "include/private/base/SkTPin.h"
#include "include/private/base/SkTemplates.h"
#include "tools/sk_app/Window.h"

#include "include/core/SkPathEffect.h"
#include "include/effects/SkDiscretePathEffect.h"
#include "include/effects/SkGradientShader.h"
#include "include/effects/SkImageFilters.h"

#include "include/core/SkFontMgr.h"
#include "include/ports/SkFontMgr_fontconfig.h"
#include "include/ports/SkFontMgr_empty.h"
#include "include/ports/SkFontMgr_data.h"
#include "include/core/SkSpan.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkFont.h"

#include "imgui.h"
#ifdef RENDER_MODE_SKETCH_ENABLED
#include "imgui_internal.h"
#endif

#include "tracy/Tracy.hpp"


template <typename T>
static void loadCmdRectRounded_(SkRect &rect, float &r, const T &cmd) {
    r = cmd.rounding();
    rect = SkRect::MakeLTRB(
            cmd.p_min()->x(),
            cmd.p_min()->y(),
            cmd.p_max()->x(),
            cmd.p_max()->y());
    //if(r < 0.5f) {
    //    r = 0.0f;
    //}
}
template <typename T>
static void loadCmdRectRoundedCorners_(SkRRect &out,const T &cmd) {
    auto const rect = SkRect::MakeLTRB(
            cmd.p_min()->x(),
            cmd.p_min()->y(),
            cmd.p_max()->x(),
            cmd.p_max()->y());
    auto const topLeft = cmd.rounding_top_left();
    auto const topRight = cmd.rounding_top_right();
    auto const bottomRight = cmd.rounding_bottom_right();
    auto const bottomLeft = cmd.rounding_bottom_left();
    SkVector corners[] = {{topLeft, topLeft },
                          {topRight, topRight},
                          {bottomRight, bottomRight},
                          {bottomLeft, bottomLeft}};
    static_assert(SkRRect::kUpperLeft_Corner == 0);
    static_assert(SkRRect::kUpperRight_Corner == 1);
    static_assert(SkRRect::kLowerRight_Corner == 2);
    static_assert(SkRRect::kLowerLeft_Corner == 3);
    out.setRectRadii(rect,corners);
}

void VectorCmdSkiaRenderer::prepareOutlinePaint(SkPaint &paint, VectorCmdFB::DrawListFlags dlFlags) {
    paint.setAntiAlias(dlFlags & VectorCmdFB::DrawListFlags_AntiAliasedLines);
    paint.setStyle(SkPaint::kStroke_Style);
#ifdef RENDER_MODE_SKETCH_ENABLED
    if(renderMode & RenderModeE_Sketch) {
        paint.setPathEffect(SkDiscretePathEffect::Make(10.0f, 1.0f));
    }
#endif
}
void VectorCmdSkiaRenderer::prepareFillPaint(SkPaint &paint, VectorCmdFB::DrawListFlags dlFlags) {
    paint.setAntiAlias(dlFlags & VectorCmdFB::DrawListFlags_AntiAliasedFill);
    paint.setStyle(SkPaint::kFill_Style);
#ifdef RENDER_MODE_SKETCH_ENABLED
    if(renderMode & RenderModeE_Sketch) {
        paint.setPathEffect(SkDiscretePathEffect::Make(10.0f, 1.0f));
    }
#endif
}
void VectorCmdSkiaRenderer::changeRenderMode(RenderModeE r) {
    renderMode = r;
}
void VectorCmdSkiaRenderer::setVertexDrawPaint(SkPaint *vertexPaintP) {
    vertexPaint = vertexPaintP;
}
void VectorCmdSkiaRenderer::setParagraphHandler(std::shared_ptr<Paragraph> paragraph) {
    fParagraph = paragraph;
    const auto typeface = paragraph->getDefaultTypeface();
    //assert(typeface != nullptr);
    fFont = SkFont(typeface);

    // TODO make this configurable?
    fFont.setEdging(SkFont::Edging::kSubpixelAntiAlias);
    fFont.setSubpixel(true);
}
RenderModeE VectorCmdSkiaRenderer::getRenderMode() {
    return renderMode;
}

static SkColor convertColor(uint32_t col) {
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
    return SkColor(col);
#else
    return  SkColorSetARGB(
        (col >> IM_COL32_A_SHIFT) & 0xff,
        (col >> IM_COL32_R_SHIFT) & 0xff,
        (col >> IM_COL32_G_SHIFT) & 0xff,
        (col >> IM_COL32_B_SHIFT) & 0xff);
#endif
}
VectorCmdSkiaRenderer::VectorCmdSkiaRenderer() {
#ifdef RENDER_MODE_BACKDROP_FILTER_ENABLED
    backdropFilter = SkImageFilters::Blur(8, 8, SkTileMode::kClamp, nullptr);
#if 0
    //SkScalar kernel[9] = {
    //    SkIntToScalar( 1), SkIntToScalar( 1), SkIntToScalar( 1),
    //    SkIntToScalar( 1), SkIntToScalar(-7), SkIntToScalar( 1),
    //    SkIntToScalar( 1), SkIntToScalar( 1), SkIntToScalar( 1),
    //};
    SkScalar kernel[9] = {
        SkIntToScalar( 1), SkIntToScalar( 2), SkIntToScalar( 1),
        SkIntToScalar( 2), SkIntToScalar( 4), SkIntToScalar( 2),
        SkIntToScalar( 1), SkIntToScalar( 2), SkIntToScalar( 1),
    };
    SkISize kernelSize = SkISize::Make(3, 3);
    SkScalar gain = SkScalar(1.0f/16.0f);
    SkScalar bias = 0;
    backdropFilter = SkImageFilters::MatrixConvolution(kernelSize,kernel,gain, bias,SkIPoint::Make(1,1),SkTileMode::kClamp,false,nullptr);
#endif
    //sk_sp<SkImageFilter> filter = SkImageFilters::DropShadow(5.0f, 5.0f, 3.0f, 3.0f, SK_ColorBLACK, nullptr, rect);

    renderMode = RenderModeE_BackdropBlur;
    fParagraph = nullptr;
#endif
}
VectorCmdSkiaRenderer::~VectorCmdSkiaRenderer() {
}

#ifdef RENDER_MODE_BACKDROP_FILTER_ENABLED
constexpr SkColor windowDecorationBlurFilterDrawColor = 0x40ffffff;
static void decorateWindowRect(SkRect const &rect,SkCanvas &canvas,sk_sp<SkImageFilter> filter) { ZoneScoped;
    SkPaint paint;
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.save();
    canvas.clipRect(rect,true);
    paint.setImageFilter(std::move(filter));

    SkCanvas::SaveLayerRec slr(&rect, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
    canvas.saveLayer(slr);
    canvas.drawColor(windowDecorationBlurFilterDrawColor);
}
static void decorateWindowRRect(SkRRect const &rrect,SkCanvas &canvas,sk_sp<SkImageFilter> filter) { ZoneScoped;
    SkPaint paint;
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.save();
    canvas.clipRRect(rrect,true);
    paint.setImageFilter(std::move(filter));

    auto const rect = rrect.rect();
    SkCanvas::SaveLayerRec slr(&rect, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
    canvas.saveLayer(slr);
    canvas.drawColor(windowDecorationBlurFilterDrawColor);
}
void VectorCmdSkiaRenderer::setupWindowRectPaint(SkPaint &paint, VectorCmdFB::DrawListFlags dlFlags, uint32_t col) { ZoneScoped;
    constexpr uint8_t windowBgColorA = 172;
    prepareFillPaint(paint,dlFlags);
    auto color = convertColor(col);
    if((renderMode & RenderModeE_SVG) == 0) {
        color =  SkColorSetA(color, windowBgColorA);
    }
    paint.setColor(color);
}
#endif
void VectorCmdSkiaRenderer::drawSerializedVectorCmdsFB(const uint8_t *buf, SkCanvas &canvas) { ZoneScoped;
        auto drawListFb = VectorCmdFB::GetDrawList(buf);
        drawVectorCmdsFBDrawList(drawListFb,canvas,false);
}

void VectorCmdSkiaRenderer::loadVertices(const VectorCmdFB::VertexData *vertices) { ZoneScoped;
    vtxColors.clear();
    vtxXYs.clear();
    vtxTexUVs.clear();
    vtxIndices.clear();
    if(vertices == nullptr) {
        return;
    }
    auto vtxColorsFB = vertices->col();
    auto vtxXYsFB = vertices->pos_xy();
    auto vtxTexUVsFB = vertices->texture_uv();
    auto vtxIndicesFB = vertices->indices();
    auto sz = vtxColorsFB->size();
    auto szIndices = vtxIndicesFB->size();
    assert(2*sz == vtxXYsFB->size() && "col and pos_xy must be co-arrays");
    assert(2*sz == vtxTexUVsFB->size() && "col and texture_uv must be co-arrays");
    vtxColors.reserve(sz);
    vtxXYs.reserve(sz);
    vtxTexUVs.reserve(sz);
    vtxIndices.reserve(szIndices);
    for(decltype(sz) i = 0;i < sz;i++) {
        vtxColors.push_back(vtxColorsFB->Get(i));
        vtxXYs.push_back(SkPoint::Make(vtxXYsFB->Get(2*i),vtxXYsFB->Get(2*i+1)));
        vtxTexUVs.push_back(SkPoint::Make(vtxTexUVsFB->Get(2*i),vtxTexUVsFB->Get(2*i+1)));
    }
    for(decltype(szIndices) i = 0;i < szIndices;i++) {
        vtxIndices.push_back(vtxIndicesFB->Get(i));
    }
#ifndef IMGUI_USE_BGRA_PACKED_COLOR
    // ImGui colors are RGBA
    // not contained in loop as SkSwapRB may benefit from SIMD instructions
    SkSwapRB(vtxColors.begin(), vtxColors.begin(), vtxColors.size());
#endif
}

void VectorCmdSkiaRenderer::drawVectorCmdsFBDrawList(const VectorCmdFB::DrawList *drawListFb, SkCanvas &canvas, bool inner) { ZoneScoped;
        auto cmds = drawListFb->cmds();
        auto n = static_cast<int>(cmds->size());
        auto dlFlags = drawListFb->flags();
        SkAutoCanvasRestore acr(&canvas, true);
        int s = 0;
        if(!inner) {
            ZoneText(drawListFb->name()->c_str(),drawListFb->name()->size());
        }
        
        loadVertices(drawListFb->vertices());

#ifdef RENDER_MODE_BACKDROP_FILTER_ENABLED
        if(!inner && (renderMode & RenderModeE_BackdropBlur) != 0) {
            int n1 = std::min(n,5);
            for(int i=0;i<n1 && s == 0;i++) {
                auto const cmdUnion = cmds->Get(i);
                switch(cmdUnion->arg_type()) {
                    case VectorCmdFB::VectorCmdArg_CmdRectRoundedFilled:
                        // special dispatch for drawing windows
                        {
                            auto const cmd = cmdUnion->arg_as_CmdRectRoundedFilled();
                            SkRect rect;
                            float r;
                            SkPaint paint;
                            loadCmdRectRounded_(rect,r,*cmd);
                            if(r > 0.0f) {
                                decorateWindowRRect(SkRRect::MakeRectXY(rect,r,r),canvas,backdropFilter);
                            } else {
                                decorateWindowRect(rect,canvas,backdropFilter);
                            }
                            setupWindowRectPaint(paint,dlFlags,cmd->col());
                            drawRectRounded(rect,r,canvas,paint);
                            s = i+1;
                        }
                        break;
                    case VectorCmdFB::VectorCmdArg_CmdRectRoundedCornersFilled:
                        // special dispatch for drawing windows
                        {
                            auto const cmd = cmdUnion->arg_as_CmdRectRoundedCornersFilled();
                            SkRRect rrect;
                            SkPaint paint;
                            loadCmdRectRoundedCorners_(rrect,*cmd);
                            decorateWindowRRect(rrect,canvas,backdropFilter);
                            setupWindowRectPaint(paint,dlFlags,cmd->col());
                            drawRRect(rrect,canvas,paint);
                            s = i+1;
                        }
                        break;
                    default:
                        drawVectorCmdFB(cmdUnion,canvas,dlFlags);
                }
            }
        }
#endif
    for(int i=s;i<n;i++) {
        auto const cmdUnion = cmds->Get(i);
        drawVectorCmdFB(cmdUnion,canvas,dlFlags);
    }
}
void VectorCmdSkiaRenderer::drawVectorCmdFB(const VectorCmdFB::SingleVectorCmdDto *cmdUnion, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    auto t = cmdUnion->arg_type();
    switch(t) {
        case VectorCmdFB::VectorCmdArg_CmdPolyline:
            drawCmdPolylineFB(*cmdUnion->arg_as_CmdPolyline(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdConvexPolyFilled:
            drawCmdConvexPolylineFilledFB(*cmdUnion->arg_as_CmdConvexPolyFilled(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdLine:
            drawCmdLineFB(*cmdUnion->arg_as_CmdLine(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdRectRounded:
            drawCmdRectRoundedFB(*cmdUnion->arg_as_CmdRectRounded(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdRectRoundedFilled:
            drawCmdRectRoundedFilledFB(*cmdUnion->arg_as_CmdRectRoundedFilled(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdRectRoundedCorners:
            drawCmdRectRoundedCornersFB(*cmdUnion->arg_as_CmdRectRoundedCorners(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdRectRoundedCornersFilled:
            drawCmdRectRoundedCornersFilledFB(*cmdUnion->arg_as_CmdRectRoundedCornersFilled(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdQuad:
            drawCmdQuadFB(*cmdUnion->arg_as_CmdQuad(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdQuadFilled:
            drawCmdQuadFilledFB(*cmdUnion->arg_as_CmdQuadFilled(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdTriangle:
            drawCmdTriangleFB(*cmdUnion->arg_as_CmdTriangle(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdTriangleFilled:
            drawCmdTriangleFilledFB(*cmdUnion->arg_as_CmdTriangleFilled(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdCircle:
            drawCmdCircleFB(*cmdUnion->arg_as_CmdCircle(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdCircleFilled:
            drawCmdCircleFilledFB(*cmdUnion->arg_as_CmdCircleFilled(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdNgon: 
            drawCmdNgonFB(*cmdUnion->arg_as_CmdNgon(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdNgonFilled:
            drawCmdNgonFilledFB(*cmdUnion->arg_as_CmdNgonFilled(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdEllipse:
            drawCmdEllipseFB(*cmdUnion->arg_as_CmdEllipse(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdEllipseFilled:
            drawCmdEllipseFilledFB(*cmdUnion->arg_as_CmdEllipseFilled(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdBezierCubic:
            drawCmdBezierCubicFB(*cmdUnion->arg_as_CmdBezierCubic(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdBezierQuadratic:
            drawCmdBezierQuadraticFB(*cmdUnion->arg_as_CmdBezierQuadratic(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdImage: break;
        case VectorCmdFB::VectorCmdArg_CmdImageQuad: break;
        case VectorCmdFB::VectorCmdArg_CmdImageRounded: break;
        case VectorCmdFB::VectorCmdArg_CmdPushClipRect:
            handleCmdPushClipRect(*cmdUnion->arg_as_CmdPushClipRect(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdPopClipRect:
            handleCmdPopClipRect(*cmdUnion->arg_as_CmdPopClipRect(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdRenderText:
            drawCmdRenderTextFB(*cmdUnion->arg_as_CmdRenderText(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdRectFilledMultiColor:
            drawCmdRectFilledMultiColorFB(*cmdUnion->arg_as_CmdRectFilledMultiColor(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdVertexDraw: 
            drawCmdVertexDraw(*cmdUnion->arg_as_CmdVertexDraw(),canvas,dlFlags);
            break;
        case VectorCmdFB::VectorCmdArg_CmdWrappedDrawList: 
            drawVectorCmdsFBDrawList(cmdUnion->arg_as_CmdWrappedDrawList()->buffer_nested_root(),canvas,true);
            break;
        case VectorCmdFB::VectorCmdArg_CmdRegisterFont: 
            registerFont(*cmdUnion->arg_as_CmdRegisterFont(),canvas,dlFlags);
            break;
        default:
            // skipping unknown/invalid command
            ;
    }
}
template <typename T>
static inline void drawCmdPolyline_(const T &cmd,SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
    paint.setColor(convertColor(cmd.col()));
    SkPath pa;
    auto const xs = cmd.points()->xs();
    auto const ys = cmd.points()->ys();
    auto const n = static_cast<int>(xs->size());
    if(n == 0) {
        return;
    }
    auto x0 = SkScalar(xs->Get(0));
    auto y0 = SkScalar(ys->Get(0));
    pa.moveTo(x0,y0);
    for(int i = 1;i<n;i++) {
        pa.lineTo(SkScalar(xs->Get(i)),SkScalar(ys->Get(i)));
    }
    canvas.drawPath(pa,paint);
}
void VectorCmdSkiaRenderer::drawCmdPolylineFB(const VectorCmdFB::CmdPolyline &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(SkScalar(cmd.thickness()));
    drawCmdPolyline_(cmd,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdConvexPolylineFilledFB(const VectorCmdFB::CmdConvexPolyFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdPolyline_(cmd,canvas,paint);
}

template <typename T>
static inline void drawCmdNgon_(const T &cmd,SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
    paint.setColor(convertColor(cmd.col()));
    auto const ce = SkPoint::Make(cmd.center()->x(),cmd.center()->y());
    auto const n = cmd.num_segments();
    auto const r =  cmd.radius();
    SkPath pa;
    pa.moveTo(ce + SkPoint::Make(SkScalar(r),SkScalar(0.0)));
    for (int i = 1; i <= n; i++) {
        SkScalar angle = 2 * SK_ScalarPI * i / n;
        SkPoint p = { SkScalarCos(angle), SkScalarSin(angle) };
        p.scale(r);
        p = ce + p;
        pa.lineTo(p);
    }
    pa.close();
    canvas.drawPath(pa,paint);
}
void VectorCmdSkiaRenderer::drawCmdNgonFB(const VectorCmdFB::CmdNgon &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(SkScalar(cmd.thickness()));
    drawCmdNgon_(cmd,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdNgonFilledFB(const VectorCmdFB::CmdNgonFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdNgon_(cmd,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdLineFB(const VectorCmdFB::CmdLine &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    paint.setStrokeWidth(SkScalar(cmd.thickness()));
    canvas.drawLine(SkPoint::Make(cmd.p1()->x(),cmd.p1()->y()),SkPoint::Make(cmd.p2()->x(),cmd.p2()->y()),paint);
}

void VectorCmdSkiaRenderer::drawRectRounded(const SkRect &rect, float r,SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
#ifdef RENDER_MODE_SKETCH_ENABLED
    if(renderMode & (RenderModeE_Sketch | RenderModeE_SVG)) {
        SkPath pa;
        if(r > 0.0f) {
            pa.addRoundRect(rect,r,r);
        } else {
            pa.addRect(rect);
        }
        canvas.drawPath(pa,paint);
        return;
    }
#endif
    if(r > 0.0f) {
        canvas.drawRoundRect(rect,r,r,paint);
    } else {
        canvas.drawRect(rect,paint);
    }
}
void VectorCmdSkiaRenderer::drawRRect(const SkRRect &rect, SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
#ifdef RENDER_MODE_SKETCH_ENABLED
    if(renderMode & (RenderModeE_Sketch | RenderModeE_SVG)) {
        SkPath pa;
        pa.addRRect(rect);
        canvas.drawPath(pa,paint);
        return;
    }
#endif
    
    canvas.drawRRect(rect,paint);
}
void VectorCmdSkiaRenderer::drawCmdRectRoundedFB(const VectorCmdFB::CmdRectRounded &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    SkRect rect;
    float r;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(cmd.thickness());
    paint.setColor(convertColor(cmd.col()));
    loadCmdRectRounded_(rect,r,cmd);
    drawRectRounded(rect,r,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdRectRoundedFilledFB(const VectorCmdFB::CmdRectRoundedFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    SkRect rect;
    float r;
    prepareFillPaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    loadCmdRectRounded_(rect,r,cmd);
    drawRectRounded(rect,r,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdRectRoundedCornersFB(const VectorCmdFB::CmdRectRoundedCorners &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    SkRRect rect;
    prepareOutlinePaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    paint.setStrokeWidth(cmd.thickness());
    loadCmdRectRoundedCorners_(rect,cmd);
    drawRRect(rect,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdRectRoundedCornersFilledFB(const VectorCmdFB::CmdRectRoundedCornersFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    SkRRect rect;
    prepareFillPaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    loadCmdRectRoundedCorners_(rect,cmd);
    drawRRect(rect,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdRectFilledMultiColorFB(const VectorCmdFB::CmdRectFilledMultiColor &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPoint pts1[] = { {cmd.p_min()->x(), cmd.p_min()->y()},
                      {cmd.p_min()->x(), cmd.p_max()->y()} };
    SkPoint pts2[] = { {cmd.p_min()->x(), cmd.p_min()->y()},
                      {cmd.p_max()->x(), cmd.p_max()->y()} };
    SkColor colors1[] = {convertColor(cmd.col_upr_left()), convertColor(cmd.col_bot_left())};
    SkColor colors2[] = {convertColor(cmd.col_bot_left()), convertColor(cmd.col_bot_right())};
    SkScalar pos[] = {SkScalar(0.0f), SkScalar(1.0f)};
    auto sh1 = SkGradientShader::MakeLinear(pts1,colors1,pos,2,SkTileMode::kDecal);
    auto sh2 = SkGradientShader::MakeLinear(pts2,colors2,pos,2,SkTileMode::kDecal);
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    paint.setShader(SkShaders::Blend(SkBlendMode::kMultiply, sh1, sh2));
    canvas.drawRect(SkRect::MakeLTRB(
        cmd.p_min()->x(),
        cmd.p_min()->y(),
        cmd.p_max()->x(),
        cmd.p_max()->y()),
        paint);
}
void VectorCmdSkiaRenderer::registerFont(const VectorCmdFB::CmdRegisterFont &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    const auto name  = cmd.name();
    const auto url = cmd.url();
    const auto urlLen = url->size();
    const auto urlCStr = url->c_str();

    if(fParagraph != nullptr) {
        fParagraph = nullptr;
    }
    
    sk_sp<SkData> fontData;
    {
        constexpr auto pfx = "file://";
        const auto pfxLen = strlen(pfx);
        if(urlLen > pfxLen && memcmp(urlCStr,pfx,pfxLen) == 0) {
            const auto path = urlCStr+pfxLen;
            fontData = SkData::MakeFromFileName(path);
        } else {
            // TODO handle error
            return;
        }
    }

    auto fontMgr = SkFontMgr_New_Custom_Data(SkSpan<sk_sp<SkData>>(&fontData,1));
    const auto typeface = fontMgr->makeFromData(fontData);
    assert(typeface != nullptr);
    setParagraphHandler(std::make_shared<Paragraph>(fontMgr, typeface));

    const auto aa = (dlFlags & VectorCmdFB::DrawListFlags_AntiAliasedText) != 0;
    if(cmd.subpixel()) {
        if(aa) {
            fFont.setEdging(SkFont::Edging::kAlias);
        } else {
            fFont.setEdging(SkFont::Edging::kSubpixelAntiAlias);
        }
    } else {
        if(aa) {
            fFont.setEdging(SkFont::Edging::kAntiAlias);
        } else {
            fFont.setEdging(SkFont::Edging::kAlias);
        }
    }
}
void VectorCmdSkiaRenderer::drawCmdVertexDraw(const VectorCmdFB::CmdVertexDraw &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    if(vertexPaint == nullptr) {
        return;
    }    

    auto cr = cmd.clip_rect();
    auto elementCount = cmd.element_count();
    auto indexOffset = cmd.index_offset();
    auto vtxOffset = cmd.vtx_offset();

    SkAutoCanvasRestore acr(&canvas, true);
    canvas.clipRect(SkRect::MakeLTRB(cr->x(), cr->y(), cr->z(), cr->w()));
    auto vertices = SkVertices::MakeCopy(SkVertices::kTriangles_VertexMode,
                                            vtxXYs.size()-vtxOffset,
                                            vtxXYs.begin()+vtxOffset,
                                            vtxTexUVs.begin()+vtxOffset,
                                            vtxColors.begin()+vtxOffset,
                                            elementCount,
                                            vtxIndices.begin() + indexOffset);
    canvas.drawVertices(vertices, SkBlendMode::kModulate, *vertexPaint);
}
template <typename T>
static void inline drawCmdQuad_(const T &cmd,SkCanvas &canvas, SkPaint &paint) { ZoneScoped;
    SkPath pa;
    paint.setColor(convertColor(cmd.col()));
    auto const p1x = cmd.p1()->x();
    auto const p1y = cmd.p1()->y();
    pa.moveTo(p1x,p1y);
    pa.lineTo(cmd.p2()->x(),cmd.p2()->y());
    pa.lineTo(cmd.p3()->x(),cmd.p3()->y());
    pa.lineTo(cmd.p4()->x(),cmd.p4()->y());
    pa.lineTo(p1x,p1y);
    pa.close();
    canvas.drawPath(pa,paint);
}
void VectorCmdSkiaRenderer::drawCmdQuadFB(const VectorCmdFB::CmdQuad &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(cmd.thickness());
    drawCmdQuad_(cmd,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdQuadFilledFB(const VectorCmdFB::CmdQuadFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdQuad_(cmd,canvas,paint);
}
template <typename T>
static inline void drawCmdTriangle_(const T &cmd,SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
    SkPath pa;
    paint.setColor(convertColor(cmd.col()));
    auto const p1x = cmd.p1()->x();
    auto const p1y = cmd.p1()->y();
    pa.moveTo(p1x,p1y);
    pa.lineTo(cmd.p2()->x(),cmd.p2()->y());
    pa.lineTo(cmd.p3()->x(),cmd.p3()->y());
    pa.lineTo(p1x,p1y);
    pa.close();
    canvas.drawPath(pa,paint);
}
void VectorCmdSkiaRenderer::drawCmdTriangleFB(const VectorCmdFB::CmdTriangle &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(cmd.thickness());
    drawCmdTriangle_(cmd,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdTriangleFilledFB(const VectorCmdFB::CmdTriangleFilled &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdTriangle_(cmd,canvas,paint);
}

void VectorCmdSkiaRenderer::drawCmdRenderTextFB(const VectorCmdFB::CmdRenderText &cmd,SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    auto const cpu_fine_clip = cmd.cpu_fine_clip();
    if(cpu_fine_clip) {
        canvas.save();
        auto const &cr = cmd.clip_rect();
        canvas.clipRect(SkRect::MakeLTRB(cr->x(), cr->y(), cr->z(),cr->w()));
    }
    paint.setColor(convertColor(cmd.col()));
    auto size = cmd.size();
    auto font = fFont.makeWithSize(SkScalar(size));
    const auto text = cmd.text();
    if(cmd.is_paragraph()){ ZoneScoped;
        fParagraph->setForegroundPaint(paint);
        fParagraph->build(text->data(), text->size());
        const auto ww = cmd.wrap_width();
        assert(ww > 0.0f && "wrap width is expected to be positive in case of is_paragraph=true");
        fParagraph->layout(SkScalar(ww));
        fParagraph->paint(canvas, SkScalar(cmd.pos()->x()), SkScalar(cmd.pos()->y()));
    } else { ZoneScoped;
        float dy = 0.0f;
        { ZoneScoped;
            SkFontMetrics metrics;
            font.getMetrics(&metrics);
            dy = fabs(SkScalarToFloat(metrics.fAscent)) + size*ImGui::skiaFontDyFudge;
        }
        canvas.drawSimpleText(text->data(),
                               text->size(),
                               SkTextEncoding::kUTF8,
                               cmd.pos()->x(),
                               cmd.pos()->y()+dy,
                               font,
                               paint);
    }

    if(cpu_fine_clip) { ZoneScoped;
        canvas.restore();
    }
}
template <typename T>
void drawCmdCircle_(const T &cmd,SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
    paint.setColor(convertColor(cmd.col()));
    canvas.drawCircle(SkScalar(cmd.center()->x()),SkScalar(cmd.center()->y()),SkScalar(cmd.radius()),paint);
}
template <typename T>
void drawCmdCircleAsPath_(const T &cmd,SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
    paint.setColor(convertColor(cmd.col()));
    SkPath pa;
    pa.addCircle(SkScalar(cmd.center()->x()),SkScalar(cmd.center()->y()),SkScalar(cmd.radius()));
    canvas.drawPath(pa,paint);
}
void VectorCmdSkiaRenderer::drawCmdCircleFB(const VectorCmdFB::CmdCircle &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(SkScalar(cmd.thickness()));
#ifdef RENDER_MODE_SKETCH_ENABLED
    if(renderMode & (RenderModeE_Sketch | RenderModeE_SVG)) {
        drawCmdCircleAsPath_(cmd,canvas,paint);
        return;
    }
#endif
    drawCmdCircle_(cmd,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdCircleFilledFB(const VectorCmdFB::CmdCircleFilled &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
#ifdef RENDER_MODE_SKETCH_ENABLED
    if(renderMode & (RenderModeE_Sketch | RenderModeE_SVG)) {
        drawCmdCircleAsPath_(cmd,canvas,paint);
        return;
    }
#endif
    drawCmdCircle_(cmd,canvas,paint);
}
void VectorCmdSkiaRenderer::handleCmdPushClipRect(const VectorCmdFB::CmdPushClipRect &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    auto crV4 = cmd.rect();
    auto cr = SkRect::MakeLTRB(crV4->x(),crV4->y(),crV4->z(),crV4->w());
    auto const intersect = cmd.intersect_with_current_clip_rect();

    if(!intersect) {
        auto n = canvas.getSaveCount();
        canvas.restoreToCount(n);
    }
    canvas.save();
    canvas.clipRect(cr,false);
}
void VectorCmdSkiaRenderer::handleCmdPopClipRect(const VectorCmdFB::CmdPopClipRect &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    canvas.restore();
}
template <typename T>
void drawCmdEllipse_(const T &cmd,SkCanvas &canvas,SkPaint &paint) {
    paint.setColor(convertColor(cmd.col()));
    auto const x = SkScalar(cmd.center()->x());
    auto const y = SkScalar(cmd.center()->y());
    auto const r = SkRadiansToDegrees(SkScalar(cmd.rot()));
    canvas.rotate(r,x,y);

    auto const radius_x = SkScalar(cmd.radius_x());
    auto const radius_y = SkScalar(cmd.radius_y());
    canvas.drawOval(SkRect::MakeLTRB(x-radius_x,y-radius_y,x+radius_x,y+radius_y),paint);
    canvas.rotate(-r,x,y);
}
void VectorCmdSkiaRenderer::drawCmdEllipseFB(const VectorCmdFB::CmdEllipse &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(SkScalar(cmd.thickness()));
    drawCmdEllipse_(cmd,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdEllipseFilledFB(const VectorCmdFB::CmdEllipseFilled &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdEllipse_(cmd,canvas,paint);
}
void VectorCmdSkiaRenderer::drawCmdBezierCubicFB(const VectorCmdFB::CmdBezierCubic &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    paint.setStrokeWidth(SkScalar(cmd.thickness()));
    SkPath pa;
    pa.moveTo(SkScalar(cmd.p1()->x()),SkScalar(cmd.p1()->y()));
    pa.cubicTo(SkScalar(cmd.p2()->x()),SkScalar(cmd.p2()->y()),
               SkScalar(cmd.p3()->x()),SkScalar(cmd.p3()->y()),
               SkScalar(cmd.p4()->x()),SkScalar(cmd.p4()->y()));
    canvas.drawPath(pa,paint);
}
void VectorCmdSkiaRenderer::drawCmdBezierQuadraticFB(const VectorCmdFB::CmdBezierQuadratic &cmd, SkCanvas &canvas,VectorCmdFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    paint.setStrokeWidth(SkScalar(cmd.thickness()));
    SkPath pa;
    pa.moveTo(SkScalar(cmd.p1()->x()),SkScalar(cmd.p1()->y()));
    pa.conicTo(SkPoint::Make(SkScalar(cmd.p2()->x()),SkScalar(cmd.p2()->y())),
               SkPoint::Make(SkScalar(cmd.p3()->x()),SkScalar(cmd.p3()->y())),SkScalar(1.0f));
    canvas.drawPath(pa,paint);
}