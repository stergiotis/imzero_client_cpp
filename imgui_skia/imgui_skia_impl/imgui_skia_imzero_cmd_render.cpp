#include "imgui_skia_imzero_cmd_render.h"

#include "include/core/SkSurface.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRRect.h"
#include "include/core/SkVertices.h"

#include "include/core/SkPathEffect.h"
#include "include/effects/SkDiscretePathEffect.h"
#include "include/effects/SkGradientShader.h"
#include "include/effects/SkImageFilters.h"
#include "include/utils/SkParsePath.h"

#include "include/ports/SkFontMgr_data.h"
#include "include/core/SkSpan.h"

#include "imgui.h"

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

void ImGuiSkia::VectorCmdSkiaRenderer::prepareOutlinePaint(SkPaint &paint, ImZeroFB::DrawListFlags dlFlags) const {
    paint.setAntiAlias(dlFlags & ImZeroFB::DrawListFlags_AntiAliasedLines);
    paint.setStyle(SkPaint::kStroke_Style);
#ifdef RENDER_MODE_SKETCH_ENABLED
    if(fRenderMode & RenderModeE_Sketch) {
        paint.setPathEffect(SkDiscretePathEffect::Make(10.0f, 1.0f));
    }
#endif
}
void ImGuiSkia::VectorCmdSkiaRenderer::prepareFillPaint(SkPaint &paint, ImZeroFB::DrawListFlags dlFlags) const {
    paint.setAntiAlias(dlFlags & ImZeroFB::DrawListFlags_AntiAliasedFill);
    paint.setStyle(SkPaint::kFill_Style);
#ifdef RENDER_MODE_SKETCH_ENABLED
    if(fRenderMode & RenderModeE_Sketch) {
        paint.setPathEffect(SkDiscretePathEffect::Make(10.0f, 1.0f));
    }
#endif
}
void ImGuiSkia::VectorCmdSkiaRenderer::prepareFillAndOutlinePaint(SkPaint &paint, ImZeroFB::DrawListFlags dlFlags) const {
    paint.setAntiAlias(dlFlags & ImZeroFB::DrawListFlags_AntiAliasedFill);
    paint.setStyle(SkPaint::kStrokeAndFill_Style);
#ifdef RENDER_MODE_SKETCH_ENABLED
    if(fRenderMode & RenderModeE_Sketch) {
        paint.setPathEffect(SkDiscretePathEffect::Make(10.0f, 1.0f));
    }
#endif
}
void ImGuiSkia::VectorCmdSkiaRenderer::changeRenderMode(RenderModeE r) {
    fRenderMode = r;
}
void ImGuiSkia::VectorCmdSkiaRenderer::setVertexDrawPaint(SkPaint *vertexPaintP) {
    fVertexPaint = vertexPaintP;
}
void ImGuiSkia::VectorCmdSkiaRenderer::setParagraphHandler(std::shared_ptr<Paragraph> paragraph) {
    fParagraph = paragraph;
    const auto typeface = paragraph->getDefaultTypeface();
    //assert(typeface != nullptr);
    fFont = SkFont(typeface);

    // TODO make this configurable?
    fFont.setEdging(SkFont::Edging::kSubpixelAntiAlias);
    fFont.setSubpixel(true);
}
ImGuiSkia::RenderModeE ImGuiSkia::VectorCmdSkiaRenderer::getRenderMode() const {
    return fRenderMode;
}

static SkColor convertColor(uint32_t col) {
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
    if constexpr (std::endian::native == std::endian::big) {
        // TODO correct?
        return static_cast<SkColor>(col);
    } else if constexpr (std::endian::native == std::endian::little) {
        return static_cast<SkColor>(col);
    } else {
        static_assert("unknown/unhandled endianness");
    }
#else
    return  SkColorSetARGB(
        (col >> IM_COL32_A_SHIFT) & 0xff,
        (col >> IM_COL32_R_SHIFT) & 0xff,
        (col >> IM_COL32_G_SHIFT) & 0xff,
        (col >> IM_COL32_B_SHIFT) & 0xff);
#endif
}

ImGuiSkia::VectorCmdSkiaRenderer::VectorCmdSkiaRenderer() : fVertexPaint(nullptr),fRenderMode(RenderModeE_Normal) {
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
    SkScalar gain = SkScalarToFloat(1.0f/16.0f);
    SkScalar bias = 0;
    backdropFilter = SkImageFilters::MatrixConvolution(kernelSize,kernel,gain, bias,SkIPoint::Make(1,1),SkTileMode::kClamp,false,nullptr);
#endif
    //sk_sp<SkImageFilter> filter = SkImageFilters::DropShadow(5.0f, 5.0f, 3.0f, 3.0f, SK_ColorBLACK, nullptr, rect);

    fRenderMode = RenderModeE_BackdropBlur;
    fParagraph = nullptr;
    fVertexPaint = nullptr;
#endif
}

ImGuiSkia::VectorCmdSkiaRenderer::~VectorCmdSkiaRenderer() {
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
void ImGuiSkia::VectorCmdSkiaRenderer::setupWindowRectPaint(SkPaint &paint, ImZeroFB::DrawListFlags dlFlags, uint32_t col) { ZoneScoped;
    prepareFillPaint(paint,dlFlags);
    auto color = convertColor(col);
    if((fRenderMode & RenderModeE_SVG) == 0) {
        constexpr uint8_t windowBgColorA = 172;
        color =  SkColorSetA(color, windowBgColorA);
    }
    paint.setColor(color);
}
#endif
void ImGuiSkia::VectorCmdSkiaRenderer::drawSerializedVectorCmdsFB(const uint8_t *buf, SkCanvas &canvas) { ZoneScoped;
        auto drawListFb = ImZeroFB::GetSizePrefixedDrawList(buf);
        drawVectorCmdsFBDrawList(drawListFb,canvas,false);
}

void ImGuiSkia::VectorCmdSkiaRenderer::loadVertices(const ImZeroFB::VertexData *vertices) { ZoneScoped;
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
    vtxColors.reserve(static_cast<int>(sz));
    vtxXYs.reserve(static_cast<int>(sz));
    vtxTexUVs.reserve(static_cast<int>(sz));
    vtxIndices.reserve(static_cast<int>(szIndices));
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

void ImGuiSkia::VectorCmdSkiaRenderer::drawVectorCmdsFBDrawList(const ImZeroFB::DrawList *drawListFb, SkCanvas &canvas, bool inner) { ZoneScoped;
        auto cmds = drawListFb->cmds();
        auto n = static_cast<int>(cmds->size());
        auto dlFlags = drawListFb->flags();
        SkAutoCanvasRestore acr(&canvas, true);
        int s = 0;
        if(!inner) {
            ZoneText(drawListFb->name()->c_str(),drawListFb->name()->size());
        } else {
            //fprintf(stderr,"rendering inner %s\n", drawListFb->name()->c_str());
        }

        loadVertices(drawListFb->vertices());

#ifdef RENDER_MODE_BACKDROP_FILTER_ENABLED
        if(!inner && (fRenderMode & RenderModeE_BackdropBlur) != 0) {
            int n1 = std::min(n,5);
            for(int i=0;i<n1 && s == 0;i++) {
                auto const cmdUnion = cmds->Get(i);
                switch(cmdUnion->arg_type()) {
                    case ImZeroFB::VectorCmdArg_CmdRectRoundedFilled:
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
                    case ImZeroFB::VectorCmdArg_CmdRectRoundedCornersFilled:
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
void ImGuiSkia::VectorCmdSkiaRenderer::drawVectorCmdFB(const ImZeroFB::SingleVectorCmdDto *cmdUnion, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    switch(cmdUnion->arg_type()) {
        case ImZeroFB::VectorCmdArg_CmdPolyline:
            drawCmdPolylineFB(*cmdUnion->arg_as_CmdPolyline(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdConvexPolyFilled:
            drawCmdConvexPolylineFilledFB(*cmdUnion->arg_as_CmdConvexPolyFilled(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdConcavePolyFilled:
            drawCmdConcavePolylineFilledFB(*cmdUnion->arg_as_CmdConcavePolyFilled(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdLine:
            drawCmdLineFB(*cmdUnion->arg_as_CmdLine(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdRectRounded:
            drawCmdRectRoundedFB(*cmdUnion->arg_as_CmdRectRounded(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdRectRoundedFilled:
            drawCmdRectRoundedFilledFB(*cmdUnion->arg_as_CmdRectRoundedFilled(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdRectRoundedCorners:
            drawCmdRectRoundedCornersFB(*cmdUnion->arg_as_CmdRectRoundedCorners(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdRectRoundedCornersFilled:
            drawCmdRectRoundedCornersFilledFB(*cmdUnion->arg_as_CmdRectRoundedCornersFilled(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdQuad:
            drawCmdQuadFB(*cmdUnion->arg_as_CmdQuad(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdQuadFilled:
            drawCmdQuadFilledFB(*cmdUnion->arg_as_CmdQuadFilled(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdTriangle:
            drawCmdTriangleFB(*cmdUnion->arg_as_CmdTriangle(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdTriangleFilled:
            drawCmdTriangleFilledFB(*cmdUnion->arg_as_CmdTriangleFilled(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdCircle:
            drawCmdCircleFB(*cmdUnion->arg_as_CmdCircle(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdCircleFilled:
            drawCmdCircleFilledFB(*cmdUnion->arg_as_CmdCircleFilled(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdNgon: 
            drawCmdNgonFB(*cmdUnion->arg_as_CmdNgon(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdNgonFilled:
            drawCmdNgonFilledFB(*cmdUnion->arg_as_CmdNgonFilled(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdEllipse:
            drawCmdEllipseFB(*cmdUnion->arg_as_CmdEllipse(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdEllipseFilled:
            drawCmdEllipseFilledFB(*cmdUnion->arg_as_CmdEllipseFilled(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdBezierCubic:
            drawCmdBezierCubicFB(*cmdUnion->arg_as_CmdBezierCubic(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdBezierQuadratic:
            drawCmdBezierQuadraticFB(*cmdUnion->arg_as_CmdBezierQuadratic(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdImage:
            drawCmdImage(*cmdUnion->arg_as_CmdImage(), canvas, dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdImageQuad: break;
        case ImZeroFB::VectorCmdArg_CmdImageRounded: break;
        case ImZeroFB::VectorCmdArg_CmdPushClipRect:
            handleCmdPushClipRect(*cmdUnion->arg_as_CmdPushClipRect(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdPopClipRect:
            handleCmdPopClipRect(*cmdUnion->arg_as_CmdPopClipRect(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdRenderText:
            drawCmdRenderTextFB(*cmdUnion->arg_as_CmdRenderText(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdRenderUnicodeCodepoint:
            drawCmdRenderUnicodeCodepointFB(*cmdUnion->arg_as_CmdRenderUnicodeCodepoint(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdRenderParagraph:
            drawCmdRenderParagraphFB(*cmdUnion->arg_as_CmdRenderParagraph(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdRectFilledMultiColor:
            drawCmdRectFilledMultiColorFB(*cmdUnion->arg_as_CmdRectFilledMultiColor(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdVertexDraw: 
            drawCmdVertexDraw(*cmdUnion->arg_as_CmdVertexDraw(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdSimpleVertexDraw:
            drawCmdSimpleVertexDraw(*cmdUnion->arg_as_CmdSimpleVertexDraw(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdPath:
            drawCmdPath(*cmdUnion->arg_as_CmdPath(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdSvgPathSubset:
            drawCmdSvgPathSubset(*cmdUnion->arg_as_CmdSvgPathSubset(),canvas,dlFlags);
            break;
        case ImZeroFB::VectorCmdArg_CmdWrappedDrawList:
            drawVectorCmdsFBDrawList(cmdUnion->arg_as_CmdWrappedDrawList()->buffer_nested_root(),canvas,true);
            break;
        case ImZeroFB::VectorCmdArg_CmdRegisterFont: 
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
    auto x0 = SkScalarToFloat(xs->Get(0));
    auto y0 = SkScalarToFloat(ys->Get(0));
    pa.moveTo(x0,y0);
    for(int i = 1;i<n;i++) {
        pa.lineTo(SkScalarToFloat(xs->Get(i)),SkScalarToFloat(ys->Get(i)));
    }
    canvas.drawPath(pa,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdPolylineFB(const ImZeroFB::CmdPolyline &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(SkScalarToFloat(cmd.thickness()));
    drawCmdPolyline_(cmd,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdConvexPolylineFilledFB(const ImZeroFB::CmdConvexPolyFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdPolyline_(cmd,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdConcavePolylineFilledFB(const ImZeroFB::CmdConcavePolyFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdPolyline_(cmd,canvas,paint);
}

template <typename T>
static void drawCmdNgon_(const T &cmd,SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
    paint.setColor(convertColor(cmd.col()));
    auto const ce = SkPoint::Make(cmd.center()->x(),cmd.center()->y());
    auto const n = cmd.num_segments();
    auto const r =  cmd.radius();
    SkPath pa;
    pa.moveTo(ce + SkPoint::Make(SkScalarToFloat(r),SkScalarToFloat(0.0)));
    for (int i = 1; i <= n; i++) {
        SkScalar angle = 2 * SK_ScalarPI * static_cast<float>(i) / n;
        SkPoint p = { SkScalarCos(angle), SkScalarSin(angle) };
        p.scale(r);
        p = ce + p;
        pa.lineTo(p);
    }
    pa.close();
    canvas.drawPath(pa,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdNgonFB(const ImZeroFB::CmdNgon &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(SkScalarToFloat(cmd.thickness()));
    drawCmdNgon_(cmd,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdNgonFilledFB(const ImZeroFB::CmdNgonFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdNgon_(cmd,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdLineFB(const ImZeroFB::CmdLine &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    paint.setStrokeWidth(SkScalarToFloat(cmd.thickness()));
    canvas.drawLine(SkPoint::Make(cmd.p1()->x(),cmd.p1()->y()),SkPoint::Make(cmd.p2()->x(),cmd.p2()->y()),paint);
}

void ImGuiSkia::VectorCmdSkiaRenderer::drawRectRounded(const SkRect &rect, float r,SkCanvas &canvas, const SkPaint &paint) { ZoneScoped;
#if defined (RENDER_MODE_SKETCH_ENABLED) || defined (RENDER_MODE_SVG_ENABLED)
    if(fRenderMode & (RenderModeE_Sketch | RenderModeE_SVG)) {
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
void ImGuiSkia::VectorCmdSkiaRenderer::drawRRect(const SkRRect &rect, SkCanvas &canvas, const SkPaint &paint) { ZoneScoped;
#if defined (RENDER_MODE_SKETCH_ENABLED) || defined (RENDER_MODE_SVG_ENABLED)
    if(fRenderMode & (RenderModeE_Sketch | RenderModeE_SVG)) {
        SkPath pa;
        pa.addRRect(rect);
        canvas.drawPath(pa,paint);
        return;
    }
#endif
    
    canvas.drawRRect(rect,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdRectRoundedFB(const ImZeroFB::CmdRectRounded &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    SkRect rect;
    float r;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(cmd.thickness());
    paint.setColor(convertColor(cmd.col()));
    loadCmdRectRounded_(rect,r,cmd);
    drawRectRounded(rect,r,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdRectRoundedFilledFB(const ImZeroFB::CmdRectRoundedFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    SkRect rect;
    float r;
    prepareFillPaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    loadCmdRectRounded_(rect,r,cmd);
    drawRectRounded(rect,r,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdRectRoundedCornersFB(const ImZeroFB::CmdRectRoundedCorners &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    SkRRect rect;
    prepareOutlinePaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    paint.setStrokeWidth(cmd.thickness());
    loadCmdRectRoundedCorners_(rect,cmd);
    drawRRect(rect,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdRectRoundedCornersFilledFB(const ImZeroFB::CmdRectRoundedCornersFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    SkRRect rect;
    prepareFillPaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    loadCmdRectRoundedCorners_(rect,cmd);
    drawRRect(rect,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdRectFilledMultiColorFB(const ImZeroFB::CmdRectFilledMultiColor &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPoint pts1[] = { {cmd.p_min()->x(), cmd.p_min()->y()},
                      {cmd.p_min()->x(), cmd.p_max()->y()} };
    SkPoint pts2[] = { {cmd.p_min()->x(), cmd.p_min()->y()},
                      {cmd.p_max()->x(), cmd.p_max()->y()} };
    SkColor colors1[] = {convertColor(cmd.col_upr_left()), convertColor(cmd.col_bot_left())};
    SkColor colors2[] = {convertColor(cmd.col_bot_left()), convertColor(cmd.col_bot_right())};
    SkScalar pos[] = {SkScalarToFloat(0.0f), SkScalarToFloat(1.0f)};
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
void ImGuiSkia::VectorCmdSkiaRenderer::registerFont(const ImZeroFB::CmdRegisterFont &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    //const auto name  = cmd.name();
    const auto url = cmd.url();
    const auto urlLen = url->size();
    const auto urlCStr = url->c_str();

    if(fParagraph != nullptr) {
        fParagraph = nullptr;
    }
    
    {
        constexpr auto pfx = "file://";
        const auto pfxLen = strlen(pfx);
        if(urlLen > pfxLen && memcmp(urlCStr,pfx,pfxLen) == 0) {
            const auto path = urlCStr+pfxLen;
            fFontData = SkData::MakeFromFileName(path);
        } else {
            // TODO handle error
            return;
        }
    }

    fFontMgr = SkFontMgr_New_Custom_Data(SkSpan<sk_sp<SkData>>(&fFontData,1));
    fTypeface = fFontMgr->makeFromData(fFontData);
    assert(fTypeface != nullptr);
    setParagraphHandler(std::make_shared<Paragraph>(fFontMgr, fTypeface));

    const auto aa = (dlFlags & ImZeroFB::DrawListFlags_AntiAliasedText) != 0;
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
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdSvgPathSubset(const ImZeroFB::CmdSvgPathSubset &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    {
        auto const fill = cmd.fill();
        auto const stroke = cmd.stroke();
        if(fill && stroke) {
            prepareFillAndOutlinePaint(paint,dlFlags);
        } else if(fill) {
            prepareOutlinePaint(paint,dlFlags);
        } else if(stroke) {
            prepareFillPaint(paint,dlFlags);
        }
    }
    paint.setColor(convertColor(cmd.col()));

    SkPath pa;
    auto const svg = cmd.svg();
    SkParsePath::FromSVGString(svg->c_str(),&pa);
    canvas.drawPath(pa,paint);

    //auto stream = SkFILEStream(stderr);
    //pa.dump((SkWStream*)&stream,true);
    //pa.dump(nullptr,true);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdPath(const ImZeroFB::CmdPath &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    {
        auto const fill = cmd.fill();
        auto const stroke = cmd.stroke();
        if(fill && stroke) {
            prepareFillAndOutlinePaint(paint,dlFlags);
        } else if(fill) {
            prepareFillPaint(paint,dlFlags);
        } else if(stroke) {
            prepareOutlinePaint(paint,dlFlags);
        }
    }
    paint.setColor(convertColor(cmd.col()));

    auto const pointsXYFb = cmd.points_xy();
    auto const pointsXY = pointsXYFb->data();
    auto const verbsFb = cmd.verbs();
    auto const weightsFb = cmd.conic_weights();
    auto const weights = weightsFb->data();
    auto const offset = cmd.offset();
    static_assert(sizeof(ImZeroFB::PathVerb) == 1);

    static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_EvenOdd) == static_cast<int64_t>(SkPathFillType::kEvenOdd));
    static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_Winding) == static_cast<int64_t>(SkPathFillType::kWinding));
    static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_InverseEvenOdd) == static_cast<int64_t>(SkPathFillType::kInverseEvenOdd));
    static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_InverseWinding) == static_cast<int64_t>(SkPathFillType::kInverseWinding));
    static_assert(std::is_same_v<SkScalar,float>);
#if 1
    SkTDArray<SkPoint> pointsXYVec;
    int nPoints = pointsXYFb->size()/2;
    pointsXYVec.reserve(nPoints);
    for(int i=0;i<nPoints;i++) {
        pointsXYVec.push_back(SkPoint::Make(pointsXY[2*i],pointsXY[2*i+1]));
    }
    //auto pa = SkPath::Make(reinterpret_cast<const SkPoint*>(pointsXY),static_cast<int>(pointsXYFb->size()/2),
    auto pa = SkPath::Make(pointsXYVec.data(),nPoints,
                 verbsFb->data(),static_cast<int>(verbsFb->size()),
                 reinterpret_cast<const SkScalar *>(weights),static_cast<int>(weightsFb->size()),
                 static_cast<SkPathFillType>(cmd.fill_type()),true);
#else
    auto const nVerbs = verbsFb->size();
    auto verbs = reinterpret_cast<const ImZeroFB::PathVerb*>(verbsFb->data());
    SkPath pa;
    pa.setFillType(static_cast<SkPathFillType>(cmd.fill_type()));
    int p=0;
    int pw=0;
    for(int i=0;i<nVerbs;i++) {
        switch(verbs[i]) {
            case ImZeroFB::PathVerb_move:
            {
                auto x0= pointsXY[p]; p++;
                auto y0 = pointsXY[p]; p++;
                pa.moveTo(SkScalarToFloat(x0),SkScalarToFloat(y0));
                break;
            }
            case ImZeroFB::PathVerb_line:
            {
                auto x0 = pointsXY[p]; p++;
                auto y0 = pointsXY[p]; p++;
                pa.lineTo(SkScalarToFloat(x0),SkScalarToFloat(y0));
                break;
            }
            case ImZeroFB::PathVerb_quad:
            {
                auto x0 = pointsXY[p]; p++;
                auto y0 = pointsXY[p]; p++;
                auto x1 = pointsXY[p]; p++;
                auto y1 = pointsXY[p]; p++;
                pa.quadTo(SkScalarToFloat(x0),SkScalarToFloat(y0),SkScalarToFloat(x1),SkScalarToFloat(y1));
                break;
            }
            case ImZeroFB::PathVerb_conic:
            {
                auto x0 = pointsXY[p]; p++;
                auto y0 = pointsXY[p]; p++;
                auto x1 = pointsXY[p]; p++;
                auto y1 = pointsXY[p]; p++;
                auto w = weights[pw]; pw++;
                pa.conicTo(SkScalarToFloat(x0),SkScalarToFloat(y0),SkScalarToFloat(x1),SkScalarToFloat(y1),SkScalarToFloat(w));
                break;
            }
            case ImZeroFB::PathVerb_cubic:
            {
                auto x0 = pointsXY[p]; p++;
                auto y0 = pointsXY[p]; p++;
                auto x1 = pointsXY[p]; p++;
                auto y1 = pointsXY[p]; p++;
                auto x2 = pointsXY[p]; p++;
                auto y2 = pointsXY[p]; p++;
                pa.cubicTo(SkScalarToFloat(x0),SkScalarToFloat(y0),SkScalarToFloat(x1),SkScalarToFloat(y1),SkScalarToFloat(x2),SkScalarToFloat(y2));
                break;
            }
            case ImZeroFB::PathVerb_close:
                pa.close();
                break;
            case ImZeroFB::PathVerb_done:
                // FIXME correct?
                break;
        }
    }
    assert(p == cmd.points_xy()->size());
#endif
    //auto stream = SkFILEStream(stderr);
    //pa.dump((SkWStream*)&stream,true);
    //pa.dump(nullptr,true);

    pa.offset(SkScalarToFloat(offset->x()),SkScalarToFloat(offset->y()));

    canvas.drawPath(pa,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdSimpleVertexDraw(const ImZeroFB::CmdSimpleVertexDraw &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags)
{
    ZoneScoped;
    auto cr = cmd.clip_rect();
    auto const crRect = SkRect::MakeLTRB(cr->x(), cr->y(), cr->z(), cr->w());

    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    constexpr auto blendMode = SkBlendMode::kDst;

#ifdef RENDER_MODE_SVG_ENABLED
    SkCanvas *rasterCanvas = nullptr;
    sk_sp<SkSurface> rasterSurface;
    const auto dx = SkScalarToFloat(cr->x());
    const auto dy = SkScalarToFloat(cr->y());
    if(fRenderMode & RenderModeE_SVG) {
        const auto s = SkSize::Make(SkScalarToFloat(cr->z())-dx,SkScalarToFloat(cr->w())-dy);
        const auto c = SkColorInfo(kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
        rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(s.toCeil(), c));
        rasterCanvas = rasterSurface->getCanvas();
    }
#endif
    auto nVertices = static_cast<int>(cmd.pos_xy()->size()/2);
    auto const p = cmd.pos_xy()->data();

#if 0
    vtxXYSimples.clear();
    for(int i=0;i<nVertices;i++) {
        vtxXYSimples.push_back(SkPoint::Make(p[2*i],p[2*i+1]));
    }
    auto vertices = SkVertices::MakeCopy(SkVertices::kTriangles_VertexMode,
                                         nVertices,
                                         vtxXYSimples.begin(),
                                         nullptr,
                                         nullptr);
#else
    static_assert(sizeof(SkPoint) == 2*sizeof(float));
    auto vertices = SkVertices::MakeCopy(SkVertices::kTriangles_VertexMode,
                                         nVertices,
                                         reinterpret_cast<const SkPoint *>(p),
                                         nullptr,
                                         nullptr);
#endif

#ifdef RENDER_MODE_SVG_ENABLED
    if(rasterCanvas) {
        rasterCanvas->setMatrix(SkMatrix::Translate(-dx,-dy));
        rasterCanvas->drawVertices(vertices, blendMode, paint);
        sk_sp<SkImage> img(rasterSurface->makeImageSnapshot());
        canvas.drawImage(img,dx,dy);
        return;
    }
#endif

    SkAutoCanvasRestore acr(&canvas, true); // FIXME necessary
    canvas.clipRect(crRect);
    canvas.drawVertices(vertices, blendMode, paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdVertexDraw(const ImZeroFB::CmdVertexDraw &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    if(fVertexPaint == nullptr) {
        return;
    }
    auto cr = cmd.clip_rect();
    auto elementCount = cmd.element_count();
    auto indexOffset = cmd.index_offset();
    auto vtxOffset = cmd.vtx_offset();
    auto const crRect = SkRect::MakeLTRB(cr->x(), cr->y(), cr->z(), cr->w());
    constexpr auto blendMode = SkBlendMode::kModulate;

#ifdef RENDER_MODE_SVG_ENABLED
    SkCanvas *rasterCanvas = nullptr;
    sk_sp<SkSurface> rasterSurface;
    const auto dx = SkScalarToFloat(cr->x());
    const auto dy = SkScalarToFloat(cr->y());
    if(fRenderMode & RenderModeE_SVG) {
        const auto s = SkSize::Make(SkScalarToFloat(cr->z())-dx,SkScalarToFloat(cr->w())-dy);
        const auto c = SkColorInfo(kRGBA_8888_SkColorType, kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
        rasterSurface = SkSurfaces::Raster(SkImageInfo::Make(s.toCeil(), c));
        rasterCanvas = rasterSurface->getCanvas();
    }
#endif

    auto vertices = SkVertices::MakeCopy(SkVertices::kTriangles_VertexMode,
                                         static_cast<int>(vtxXYs.size()-vtxOffset),
                                         vtxXYs.begin()+vtxOffset,
                                         vtxTexUVs.begin()+vtxOffset,
                                         vtxColors.begin()+vtxOffset,
                                         static_cast<int>(elementCount),
                                         vtxIndices.begin() + indexOffset);


#ifdef RENDER_MODE_SVG_ENABLED
    if(rasterCanvas) {
        rasterCanvas->setMatrix(SkMatrix::Translate(-dx,-dy));
        rasterCanvas->drawVertices(vertices, blendMode, *fVertexPaint);
        sk_sp<SkImage> img(rasterSurface->makeImageSnapshot());
        canvas.drawImage(img,dx,dy);
        return;
    }
#endif

    SkAutoCanvasRestore acr(&canvas, true); // FIXME necessary
    canvas.clipRect(crRect);
    canvas.drawVertices(vertices, blendMode, *fVertexPaint);
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
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdQuadFB(const ImZeroFB::CmdQuad &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(cmd.thickness());
    drawCmdQuad_(cmd,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdQuadFilledFB(const ImZeroFB::CmdQuadFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
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
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdTriangleFB(const ImZeroFB::CmdTriangle &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(cmd.thickness());
    drawCmdTriangle_(cmd,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdTriangleFilledFB(const ImZeroFB::CmdTriangleFilled &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdTriangle_(cmd,canvas,paint);
}

void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdRenderParagraphFB(const ImZeroFB::CmdRenderParagraph &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;

    canvas.save();
    auto const &cr = cmd.clip_rect();
    canvas.clipRect(SkRect::MakeLTRB(cr->x(), cr->y(), cr->z(),cr->w()));

    paint.setColor(convertColor(cmd.col()));
    auto size = cmd.size();
    const auto text = cmd.text();

    fParagraph->setFontSize(SkScalarToFloat(size));
    fParagraph->setForegroundPaint(paint);
    fParagraph->setLetterSpacing(cmd.letter_spacing());
    {
        auto a = skia::textlayout::TextAlign::kLeft;
        switch(cmd.text_align()) {
            case ImZeroFB::TextAlignFlags_Left: a = skia::textlayout::TextAlign::kLeft; break;
            case ImZeroFB::TextAlignFlags_Right: a = skia::textlayout::TextAlign::kRight; break;
            case ImZeroFB::TextAlignFlags_Center: a = skia::textlayout::TextAlign::kCenter; break;
            case ImZeroFB::TextAlignFlags_Justify: a = skia::textlayout::TextAlign::kJustify; break;
            default:
                assert("unhandled text align option for paragraph");
        }
        auto d = skia::textlayout::TextDirection::kLtr;
        switch(cmd.text_direction()) {
            case ImZeroFB::TextDirection_Ltr: d = skia::textlayout::TextDirection::kLtr; break;
            case ImZeroFB::TextDirection_Rtl: d = skia::textlayout::TextDirection::kRtl; break;
            default:
                assert("unhandled text direction option for paragraph");
        }
        fParagraph->setTextLayout(a, d);
    }
    fParagraph->build(text->data(), text->size());
    const auto ww = cmd.wrap_width();
    assert(ww > 0.0f && "wrap width is expected to be positive for paragraphs");
    fParagraph->layout(SkScalarToFloat(ww));
    fParagraph->paint(canvas, SkScalarToFloat(cmd.pos()->x()), SkScalarToFloat(cmd.pos()->y()));

    canvas.restore();
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdRenderTextFB(const ImZeroFB::CmdRenderText &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;

    canvas.save();
    auto const &cr = cmd.clip_rect();
    canvas.clipRect(SkRect::MakeLTRB(cr->x(), cr->y(), cr->z(),cr->w()));

    paint.setColor(convertColor(cmd.col()));
    auto size = cmd.size();
    auto font = fFont.makeWithSize(SkScalarToFloat(size));
    const auto text = cmd.text();
    if(text == nullptr) {
        canvas.restore();
	return;
    }

    float dy;
    { ZoneScoped;
        SkFontMetrics metrics{};
        font.getMetrics(&metrics);
        dy = fabsf(SkScalarToFloat(metrics.fAscent)) + size*ImGui::skiaFontDyFudge;
    }
    canvas.drawSimpleText(text->data(),
                           text->size(),
                           SkTextEncoding::kUTF8,
                           cmd.pos()->x(),
                           cmd.pos()->y()+dy,
                           font,
                           paint);

    canvas.restore();
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdRenderUnicodeCodepointFB(const ImZeroFB::CmdRenderUnicodeCodepoint &cmd,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    paint.setColor(convertColor(cmd.col()));
    auto size = cmd.size();
    auto font = fFont.makeWithSize(SkScalarToFloat(size));
    auto const cp = cmd.codepoint();

    auto const glyph =  font.unicharToGlyph(static_cast<SkUnichar>(cp));

    float dy;
    { ZoneScoped;
        SkFontMetrics metrics{};
        font.getMetrics(&metrics);
        dy = fabsf(SkScalarToFloat(metrics.fAscent)) + size*ImGui::skiaFontDyFudge;
    }
    auto const p = SkPoint::Make(cmd.pos()->x(),cmd.pos()->y()+dy);
    canvas.drawGlyphs(1,&glyph,&p,SkPoint::Make(0.0f,0.0f),font,paint);
}
template <typename T>
void drawCmdCircle_(const T &cmd,SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
    paint.setColor(convertColor(cmd.col()));
    canvas.drawCircle(SkScalarToFloat(cmd.center()->x()),SkScalarToFloat(cmd.center()->y()),SkScalarToFloat(cmd.radius()),paint);
}
template <typename T>
void drawCmdCircleAsPath_(const T &cmd,SkCanvas &canvas,SkPaint &paint) { ZoneScoped;
    paint.setColor(convertColor(cmd.col()));
    SkPath pa;
    pa.addCircle(SkScalarToFloat(cmd.center()->x()),SkScalarToFloat(cmd.center()->y()),SkScalarToFloat(cmd.radius()));
    canvas.drawPath(pa,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdCircleFB(const ImZeroFB::CmdCircle &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(SkScalarToFloat(cmd.thickness()));
#if defined (RENDER_MODE_SKETCH_ENABLED) || defined (RENDER_MODE_SVG_ENABLED)
    if(fRenderMode & (RenderModeE_Sketch | RenderModeE_SVG)) {
        drawCmdCircleAsPath_(cmd,canvas,paint);
        return;
    }
#endif
    drawCmdCircle_(cmd,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdCircleFilledFB(const ImZeroFB::CmdCircleFilled &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
#if defined (RENDER_MODE_SKETCH_ENABLED) || defined (RENDER_MODE_SVG_ENABLED)
    if(fRenderMode & (RenderModeE_Sketch | RenderModeE_SVG)) {
        drawCmdCircleAsPath_(cmd,canvas,paint);
        return;
    }
#endif
    drawCmdCircle_(cmd,canvas,paint);
}
#ifdef SKIA_DRAW_BACKEND_DEBUG_CLIPPING
static void applyClipRect(const SkRect &cr,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags,bool intersect) {
    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    if(intersect) {
        paint.setColor(SK_ColorGREEN);
    } else {
        paint.setColor(SK_ColorRED);
    }
    canvas.drawRect(cr,paint);
#else
static void applyClipRect(const SkRect &cr,SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) {
#endif
    auto const aa = (dlFlags & ImZeroFB::DrawListFlags_AntiAliasedClipping) != 0;
    canvas.clipRect(cr,aa);
}
void ImGuiSkia::VectorCmdSkiaRenderer::handleCmdPushClipRect(const ImZeroFB::CmdPushClipRect &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    auto crV4 = cmd.rect();
    auto cr = SkRect::MakeLTRB(crV4->x(),crV4->y(),crV4->z(),crV4->w());

#ifdef SKIA_DRAW_BACKEND_DEBUG_CLIPPING
    fClipStack.push_back(std::make_pair(cr,cmd.intersected_with_current_clip_rect()));
#else
    fClipStack.push_back(cr);
#endif
    canvas.restore();
    canvas.save();

#ifdef SKIA_DRAW_BACKEND_DEBUG_CLIPPING
    auto const intersect = cmd.intersected_with_current_clip_rect();
    applyClipRect(cr,canvas,dlFlags,intersect);
#else
    applyClipRect(cr,canvas,dlFlags);
#endif
}
void ImGuiSkia::VectorCmdSkiaRenderer::handleCmdPopClipRect(const ImZeroFB::CmdPopClipRect &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    canvas.restore();
    canvas.save();
    fClipStack.pop_back();
    if(!fClipStack.empty()) {
        const auto p = fClipStack[fClipStack.size()-1];
#ifdef SKIA_DRAW_BACKEND_DEBUG_CLIPPING
        applyClipRect(p.first,canvas,dlFlags,p.second);
#else
        applyClipRect(p,canvas,dlFlags);
#endif
    }
}
template <typename T>
void drawCmdEllipse_(const T &cmd,SkCanvas &canvas,SkPaint &paint) {
    paint.setColor(convertColor(cmd.col()));
    auto const x = SkScalarToFloat(cmd.center()->x());
    auto const y = SkScalarToFloat(cmd.center()->y());
    auto const r = SkRadiansToDegrees(SkScalarToFloat(cmd.rot()));
    canvas.rotate(r,x,y);

    auto const radius_x = SkScalarToFloat(cmd.radius()->x());
    auto const radius_y = SkScalarToFloat(cmd.radius()->y());
    canvas.drawOval(SkRect::MakeLTRB(x-radius_x,y-radius_y,x+radius_x,y+radius_y),paint);
    canvas.rotate(-r,x,y);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdEllipseFB(const ImZeroFB::CmdEllipse &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setStrokeWidth(SkScalarToFloat(cmd.thickness()));
    drawCmdEllipse_(cmd,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdEllipseFilledFB(const ImZeroFB::CmdEllipseFilled &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareFillPaint(paint,dlFlags);
    drawCmdEllipse_(cmd,canvas,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdBezierCubicFB(const ImZeroFB::CmdBezierCubic &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    paint.setStrokeWidth(SkScalarToFloat(cmd.thickness()));
    SkPath pa;
    pa.moveTo(SkScalarToFloat(cmd.p1()->x()),SkScalarToFloat(cmd.p1()->y()));
    pa.cubicTo(SkScalarToFloat(cmd.p2()->x()),SkScalarToFloat(cmd.p2()->y()),
               SkScalarToFloat(cmd.p3()->x()),SkScalarToFloat(cmd.p3()->y()),
               SkScalarToFloat(cmd.p4()->x()),SkScalarToFloat(cmd.p4()->y()));
    canvas.drawPath(pa,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdImage(const ImZeroFB::CmdImage &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) {
    ZoneScoped;
    switch (1) {
        case 0:
            {
                const auto img = reinterpret_cast<SkPicture*>(cmd.user_texture_id());
                auto p_min = cmd.p_min();
                canvas.save();
                canvas.translate(SkScalarToFloat(p_min->x()),SkScalarToFloat(p_min->y()));
                canvas.drawPicture(img);
                canvas.restore();
            }
            break;
        case 1:
            {
                const auto img = reinterpret_cast<SkSurface*>(cmd.user_texture_id());
                auto p_min = cmd.p_min();
                img->draw(&canvas,SkFloatToScalar(p_min->x()),SkFloatToScalar(p_min->y()),nullptr);
            }
            break;
    }
}
void ImGuiSkia::VectorCmdSkiaRenderer::drawCmdBezierQuadraticFB(const ImZeroFB::CmdBezierQuadratic &cmd, SkCanvas &canvas,ImZeroFB::DrawListFlags dlFlags) { ZoneScoped;
    SkPaint paint;
    prepareOutlinePaint(paint,dlFlags);
    paint.setColor(convertColor(cmd.col()));
    paint.setStrokeWidth(SkScalarToFloat(cmd.thickness()));
    SkPath pa;
    pa.moveTo(SkScalarToFloat(cmd.p1()->x()),SkScalarToFloat(cmd.p1()->y()));
    pa.conicTo(SkPoint::Make(SkScalarToFloat(cmd.p2()->x()),SkScalarToFloat(cmd.p2()->y())),
               SkPoint::Make(SkScalarToFloat(cmd.p3()->x()),SkScalarToFloat(cmd.p3()->y())),SkScalarToFloat(1.0f));
    canvas.drawPath(pa,paint);
}
void ImGuiSkia::VectorCmdSkiaRenderer::prepareForDrawing() {
    fClipStack.resize(0);
}

sk_sp<SkTypeface> ImGuiSkia::VectorCmdSkiaRenderer::getTypeface() const {
    return fFont.refTypeface();
}
