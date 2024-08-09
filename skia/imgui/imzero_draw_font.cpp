#include "imzero_draw_font.h"
#include "tracy/Tracy.hpp"
#include "imzero_draw_utils.h"
#include "imgui_internal.h"

static char hiddenPwBuffer[512];
static size_t hiddenPwBufferNChars = 0;
static size_t hiddenPwBufferNBytesPerChar = 0;

ImVec2 ImFont::CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining) const
{ ZoneScoped;
    if (!text_end) {
        text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.
    }

    if(remaining != nullptr) {
        *remaining = nullptr;
    }

    if(ImGui::useVectorCmd) {
        bool freeAllocatedText = false;

        if(isPasswordFont(*this)) {
            initHiddenPwBuffer(*this);
            auto const len = static_cast<size_t>(text_end-text_begin);
            if(len > hiddenPwBufferNChars) { ZoneScopedN("slow path password text allocation");
                text_begin = static_cast<char *>(IM_ALLOC(len*hiddenPwBufferNBytesPerChar));
                // slow path, very long or high codepoints password
                for(size_t i=0;i<len;i++) {
                    memcpy(const_cast<char*>(&text_begin[i*hiddenPwBufferNBytesPerChar]), hiddenPwBuffer, hiddenPwBufferNBytesPerChar);
                }
                text_end = text_begin + len;
                freeAllocatedText = true;
            } else {
                // fast path, password fits in buffer
                text_begin = hiddenPwBuffer;
            }
            text_end = text_begin + len;
        } else if(wrap_width > 0.0f || isParagraphText(text_begin,text_end)) { ZoneScoped;
            if(wrap_width <= 0.0f) {
                wrap_width = ImGui::GetContentRegionAvail().x;
            }
            if(wrap_width <= 0.0) {
                return ImVec2(0.0f,size);
            }
            ImGui::paragraph->build(text_begin,static_cast<size_t>(text_end-text_begin));
            ImGui::paragraph->layout(SkScalar(wrap_width));
            return ImVec2(std::max(wrap_width,SkScalarToFloat(ImGui::paragraph->getMaxWidth())),
                          SkScalarToFloat(ImGui::paragraph->getHeight()));
        }

        { ZoneScoped;
            auto f = ImGui::skiaFont.makeWithSize(SkScalar(size));
            SkScalar advanceWidth = f.measureText(text_begin,text_end-text_begin,SkTextEncoding::kUTF8, nullptr);
            if(freeAllocatedText) {
                IM_FREE(const_cast<char*>(text_begin));
            }
            return ImVec2(SkScalarToFloat(advanceWidth), size);
        }
    } else {
        // FIXME
        return ImVec2(0.0f,0.0f);
    }
}

void ImFont::RenderChar(ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, ImWchar c) const
{ ZoneScoped;
    if(ImGui::useVectorCmd) {
        auto posFb = ImZeroFB::SingleVec2(pos.x,pos.y);
        auto clipRectFb = ImZeroFB::SingleVec4(0.0,0.0,0.0,0.0);
        auto arg = ImZeroFB::CreateCmdRenderUnicodeCodepoint(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(this),size,&posFb,col,&clipRectFb,static_cast<uint32_t>(c));
        draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRenderUnicodeCodepoint,arg.Union());
    } else {
        // FIXME
    }
}

void ImFont::RenderText(ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip) const
{ ZoneScoped;
    if (!text_end) {
        text_end = text_begin + strlen(text_begin);
    }

    if(ImGui::useVectorCmd) {
        auto const len = static_cast<size_t>(text_end-text_begin);
        if(pos.y > clip_rect.w) {
            return;
        }

        auto posFb = ImZeroFB::SingleVec2(pos.x,pos.y);
        auto clipRectFb = ImZeroFB::SingleVec4(clip_rect.x,clip_rect.y,clip_rect.z,clip_rect.w);
        flatbuffers::Offset<flatbuffers::String> textFb;
        if(isPasswordFont(*this)) {
            initHiddenPwBuffer(*this);
            if(hiddenPwBufferNChars == 0) {
                unsigned int cp = ImGui::skiaPasswordDefaultCharacter;
                if(this->FallbackGlyph != nullptr && this->FallbackGlyph->Codepoint > 0) {
                    cp = static_cast<unsigned int>(this->FallbackGlyph->Codepoint);
                }
                if(cp < 0x7f) {
                    // fast path: single byte character
                    hiddenPwBufferNChars = sizeof(hiddenPwBuffer);
                    memset(hiddenPwBuffer,static_cast<int>(cp),hiddenPwBufferNChars);
                    hiddenPwBufferNBytesPerChar = 1;
                } else {
                    // slow path: multibyte character
                    ImTextCharToUtf8(hiddenPwBuffer, cp);
                    hiddenPwBufferNBytesPerChar = strnlen(hiddenPwBuffer, sizeof(hiddenPwBuffer));
                    hiddenPwBufferNChars = sizeof(hiddenPwBufferNChars)/hiddenPwBufferNBytesPerChar;
                    for(size_t i=1;i<hiddenPwBufferNChars;i++) {
                        memcpy(&hiddenPwBuffer[i * hiddenPwBufferNBytesPerChar], hiddenPwBuffer, hiddenPwBufferNBytesPerChar);
                    }
                }
            }

            if(len > hiddenPwBufferNChars) {
                auto bufferLong = static_cast<char *>(IM_ALLOC(len*hiddenPwBufferNBytesPerChar));
                // slow path, very long or high codepoints password
                for(size_t i=0;i<len;i++) {
                    memcpy(&bufferLong[i*hiddenPwBufferNBytesPerChar], hiddenPwBuffer, hiddenPwBufferNBytesPerChar);
                }
                textFb = draw_list->fbBuilder->CreateString(bufferLong,len*hiddenPwBufferNBytesPerChar);
                IM_FREE(bufferLong);
            } else {
                // fast path, password fits in buffer
                textFb = draw_list->fbBuilder->CreateString(hiddenPwBuffer, len*hiddenPwBufferNBytesPerChar);
            }

            auto const arg = ImZeroFB::CreateCmdRenderText(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(this),size,&posFb,col,&clipRectFb,textFb);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRenderText,arg.Union());
        } else {
            textFb = draw_list->fbBuilder->CreateString(text_begin,len);
            auto isParagraph = wrap_width > 0.0f || isParagraphText(text_begin,text_end);
            if(isParagraph && wrap_width <= 0.0f) {
                wrap_width = ImGui::GetContentRegionAvail().x;
                if(wrap_width <= 0.0f) {
                    // skip text, not visible
                    return;
                }
            }
            if(isParagraph) {
//#define IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
#ifdef IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
                const bool renderAsParagraph = IMZERO_DRAWLIST_PARAGRAPH_AS_PATH;
#else
                constexpr bool renderAsParagraph = true;
#endif
                if(renderAsParagraph) {
                    auto const arg = ImZeroFB::CreateCmdRenderParagraph(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(this),size,&posFb,col,&clipRectFb,textFb,wrap_width,0.0f,ImZeroFB::TextAlignFlags_left);
                    draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRenderParagraph,arg.Union());
                } else { ZoneScopedN("paragraphAsPath");
#ifdef IMZERO_DRAWLIST_PARAGRAPH_AS_PATH
                    auto const clipRectSkia = SkRect::MakeLTRB(SkScalar(clip_rect.x),SkScalar(clip_rect.y),SkScalar(clip_rect.z),SkScalar(clip_rect.w));
                auto const clipRectSkiaTrans = clipRectSkia.makeOffset(-pos.x,-pos.y);

                ImGui::paragraph->setFontSize(SkScalar(size));
                ImGui::paragraph->build(text_begin,static_cast<size_t>(text_end-text_begin));
                ImGui::paragraph->layout(SkScalar(wrap_width));
                for(int lineNumber=0;;lineNumber++) {
                    bool found;
                    auto bounds = ImGui::paragraph->boundingRect(lineNumber,found);
                    if(!found) {
                        break;
                    }
                    if(!bounds.intersect(clipRectSkiaTrans)) {
                        // clipped
                        continue;
                    }

                    //draw_list->AddRect(ImVec2(pos.x+ SkScalarToFloat(bounds.top()), pos.y+SkScalarToFloat(bounds.left())),
                    //                         ImVec2(pos.x+SkScalarToFloat(bounds.right()), pos.y+SkScalarToFloat(bounds.bottom())),
                    //                         0xaa1199ff,0.0f,0,2.0f);

                    SkPath p;
                    auto unrenderedGlyphs = ImGui::paragraph->getPath(lineNumber,p);
                    /*
                    // example data
                    p.lineTo(1.0f,2.0f);
                    p.conicTo(3.0f,4.0f,5.0f,6.0f,7.0f);
                    p.cubicTo(8.0f,9.0f,10.0f,11.0f,12.0f,13.0f);
                    p.quadTo(14.0f,15.0f,16.0f,18.0f);

                    // output
                    auto stream = SkFILEStream(stderr);
                    p.dump((SkWStream*)&stream,true);
                    p.dump(nullptr,true);

                    // should produce the following output
                    path.setFillType(SkPathFillType::kWinding);
                    path.moveTo(SkBits2Float(0x00000000), SkBits2Float(0x00000000));  // 0, 0
                    path.lineTo(SkBits2Float(0x3f800000), SkBits2Float(0x40000000));  // 1, 2
                    path.conicTo(SkBits2Float(0x40400000), SkBits2Float(0x40800000), SkBits2Float(0x40a00000), SkBits2Float(0x40c00000), SkBits2Float(0x40e00000));  // 3, 4, 5, 6, 7
                    path.cubicTo(SkBits2Float(0x41000000), SkBits2Float(0x41100000), SkBits2Float(0x41200000), SkBits2Float(0x41300000), SkBits2Float(0x41400000), SkBits2Float(0x41500000));  // 8, 9, 10, 11, 12, 13
                    path.quadTo(SkBits2Float(0x41600000), SkBits2Float(0x41700000), SkBits2Float(0x41800000), SkBits2Float(0x41900000));  // 14, 15, 16, 18
                    */
#if 0
                    p.offset(SkScalar(pos.x),SkScalar(pos.y));
                auto svg = SkParsePath::ToSVGString(p);
                auto svgFb = draw_list->fbBuilder->CreateString(svg.data(),svg.size());
                auto arg = ImZeroFB::CreateCmdSvgPathSubset(*draw_list->fbBuilder,svgFb,col,true);
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdSvgPathSubset,arg.Union());
#else
                    static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_evenOdd) == static_cast<int64_t>(SkPathFillType::kEvenOdd));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_winding) == static_cast<int64_t>(SkPathFillType::kWinding));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_inverseEvenOdd) == static_cast<int64_t>(SkPathFillType::kInverseEvenOdd));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathFillType_inverseWinding) == static_cast<int64_t>(SkPathFillType::kInverseWinding));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_move) == static_cast<int64_t>( SkPath::Verb::kMove_Verb));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_line) == static_cast<int64_t>( SkPath::Verb::kLine_Verb));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_quad) == static_cast<int64_t>( SkPath::Verb::kQuad_Verb));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_conic) == static_cast<int64_t>( SkPath::Verb::kConic_Verb));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_cubic) == static_cast<int64_t>( SkPath::Verb::kCubic_Verb));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_close) == static_cast<int64_t>( SkPath::Verb::kClose_Verb));
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_done) == static_cast<int64_t>( SkPath::Verb::kDone_Verb));
                    static_assert(sizeof(ImZeroFB::PathVerb) == 1);

                    auto const nVerbs = p.countVerbs();
                    draw_list->fPathVerbBuffer.resize(0);
                    draw_list->fPathVerbBuffer.reserve(nVerbs);
                    draw_list->fPathWeightBuffer.resize(0);
                    draw_list->fPathWeightBuffer.reserve(nVerbs); // upper bound, only needed for conic
                    auto const nPoints = p.countPoints();
                    draw_list->fPathPointBuffer.resize(0);
                    draw_list->fPathPointBuffer.reserve(nPoints*2);
                    static_assert(sizeof(SkPoint) == 2*sizeof(float));

                    SkPath::Iter iter(p, false);
                    SkPoint pts[4];
                    SkPath::Verb verb;
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_move) == 0);
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_line) == 1);
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_quad) == 2);
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_conic) == 3);
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_cubic) == 4);
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_close) == 5);
                    static_assert(static_cast<int64_t>(ImZeroFB::PathVerb_done) == 6);
                    constexpr int nPointsLU[SkPath::kDone_Verb+1] = {1, /* move */
                                                                     1, /* line */
                                                                     2, /* quad */
                                                                     2, /* conic */
                                                                     3, /* cubic */
                                                                     0, /* close */
                                                                     0 /* done */
                    };

                    // NOTE: iter seems to be the only method to get conic weights.
                    // live would be much easier if methods p.getWeight(),p.getWeights() and p.getWeightCounts()
                    // would exist --> skia pull request?
                    while ((verb = iter.next(pts)) != SkPath::kDone_Verb) {
                        draw_list->fPathVerbBuffer.push_back(verb);
                        const int o = verb == SkPath::kMove_Verb ? 0 : 1;
                        // TODO optionally use SkPath::ConvertConicToQuads() to approximate conics (hyperbolic,elliptic,parabolic) using quadratic bezier curves
                        for(int i=0;i<nPointsLU[verb];i++) {
                            draw_list->fPathPointBuffer.push_back(pts[o+i].x());
                            draw_list->fPathPointBuffer.push_back(pts[o+i].y());
                        }
                        if(verb == SkPath::kConic_Verb) {
                            draw_list->fPathWeightBuffer.push_back(iter.conicWeight());
                        }
                        pts[0] = SkPoint::Make(-2.0f,-2.0f);
                        pts[1] = SkPoint::Make(-2.0f,-2.0f);
                        pts[2] = SkPoint::Make(-2.0f,-2.0f);
                        pts[3] = SkPoint::Make(-2.0f,-2.0f);
                    }

                    auto const pointXYs = draw_list->fbBuilder->CreateVector<float>(draw_list->fPathPointBuffer.Data,nPoints*2);
                    auto const verbs = draw_list->fbBuilder->CreateVector<uint8_t>(draw_list->fPathVerbBuffer.Data,nVerbs);
                    auto const weights = draw_list->fbBuilder->CreateVector<float>(draw_list->fPathWeightBuffer.Data,draw_list->fPathWeightBuffer.Size);

                    auto arg = ImZeroFB::CreateCmdPath(*draw_list->fbBuilder,
                                                          &posFb,
                                                          verbs,
                                                          pointXYs,
                                                          weights,
                                                          col,
                                                          false,
                                                          true,
                                                          static_cast<ImZeroFB::PathFillType>(p.getFillType()));
                    draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdPath,arg.Union());
#endif

                }
#endif
                }
            } else {
                auto const arg = ImZeroFB::CreateCmdRenderText(*draw_list->fbBuilder,reinterpret_cast<uint64_t>(this),size,&posFb,col,&clipRectFb,textFb);
                draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdRenderText,arg.Union());
            }
        }
    } else {
        // FIXME
    }
}