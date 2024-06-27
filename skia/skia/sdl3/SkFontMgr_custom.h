#pragma once
#include "include/core/SkData.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkSpan.h"
#include "include/core/SkTypes.h"
#include "SkStream.h"

class SkFontMgr;

/** Create a custom font manager which wraps a collection of SkData-stored fonts.
 *  This font manager uses FreeType for rendering.
 */
sk_sp<SkFontMgr> SkFontMgr_New_Custom_Data2(SkSpan<sk_sp<SkData>> datas);
sk_sp<SkFontMgr> SkFontMgr_New_Custom_Data2(SkMemoryStream *stream);
