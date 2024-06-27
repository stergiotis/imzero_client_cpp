/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/core/SkRefCnt.h"
#include "include/core/SkStream.h"
#include "src/core/SkFontDescriptor.h"
#include "src/core/SkFontScanner.h"
#include "SkFontMgr_custom.h"
#include "SkFontMgr_custom.h"
#include "src/ports/SkFontMgr_custom.h"

static void load_font_from_data(const SkFontScanner* scanner,
                                SkMemoryStream *stream, int index,
                                SkFontMgr_Custom::Families* families);


class DataFontLoader2 : public SkFontMgr_Custom::SystemFontLoader {
public:
    DataFontLoader2(sk_sp<SkData>* datas, int n) : fDatas(datas), fNum(n) {

	    SkDebugf("fDatas=%p,fNum=%d\n",fDatas,fNum);
	    SkDebugf("datas=%p\n",datas);
//	    SkDebugf("datas[0]=%p\n",datas[0].get());
	    SkDebugf("n=%d\n",n);
    }

    void loadSystemFonts(const SkFontScanner* scanner,
                         SkFontMgr_Custom::Families* families) const override
    {
        for (int i = 0; i < fNum; ++i) {
            auto stream = SkMemoryStream(fDatas[i]);
            load_font_from_data(scanner, &stream, i, families);
        }

        if (families->empty()) {
            SkFontStyleSet_Custom* family = new SkFontStyleSet_Custom(SkString());
            families->push_back().reset(family);
            family->appendTypeface(sk_make_sp<SkTypeface_Empty>());
        }
    }

    const sk_sp<SkData>* fDatas;
    const int fNum;
};
class DataFontLoader3 : public SkFontMgr_Custom::SystemFontLoader {
public:
    DataFontLoader3(SkMemoryStream *stream) : fStream(stream) {
    }

    void loadSystemFonts(const SkFontScanner* scanner,
                         SkFontMgr_Custom::Families* families) const override
    {
        load_font_from_data(scanner, fStream, 0, families);

        if (families->empty()) {
            SkFontStyleSet_Custom* family = new SkFontStyleSet_Custom(SkString());
            families->push_back().reset(family);
            family->appendTypeface(sk_make_sp<SkTypeface_Empty>());
        }
    }

    SkMemoryStream *fStream;
};

static SkFontStyleSet_Custom* find_family(SkFontMgr_Custom::Families& families,
                                          const char familyName[])
{
   for (int i = 0; i < families.size(); ++i) {
        if (families[i]->getFamilyName().equals(familyName)) {
            return families[i].get();
        }
    }
    return nullptr;
}

static void load_font_from_data(const SkFontScanner* scanner,
                                SkMemoryStream *stream,
                                int index,
                                SkFontMgr_Custom::Families* families) {
    int numFaces;
    if (!scanner->scanFile(stream, &numFaces)) {
        SkDebugf("---- failed to open <%d> as a font\n", index);
        return;
    }
    for (int faceIndex = 0; faceIndex < numFaces; ++faceIndex) {
        int numInstances;
        if (!scanner->scanFace(stream, faceIndex, &numInstances)) {
            SkDebugf("---- failed to open <%d> <%d> as a face\n", index, faceIndex);
            continue;
        }

        for (int instanceIndex = 0; instanceIndex <= numInstances; ++instanceIndex) {
            bool isFixedPitch;
            SkString realname;
            SkFontStyle style = SkFontStyle();  // avoid uninitialized warning
            if (!scanner->scanInstance(stream,
                                      faceIndex,
                                      instanceIndex,
                                      &realname,
                                      &style,
                                      &isFixedPitch,
                                      nullptr)) {
                SkDebugf("---- failed to open <%d> <%d> <%d> as an instance\n",
                         index,
                         faceIndex,
                         instanceIndex);
                return;
            }

            SkFontStyleSet_Custom* addTo = find_family(*families, realname.c_str());
            if (nullptr == addTo) {
                addTo = new SkFontStyleSet_Custom(realname);
                families->push_back().reset(addTo);
            }
            auto data = std::make_unique<SkFontData>(
                    stream->duplicate(), faceIndex, 0, nullptr, 0, nullptr, 0);
            addTo->appendTypeface(sk_make_sp<SkTypeface_FreeTypeStream>(
                    std::move(data), realname, style, isFixedPitch));
        }
    }
}

sk_sp<SkFontMgr> SkFontMgr_New_Custom_Data2(SkSpan<sk_sp<SkData>> datas) {
    SkASSERT(!datas.empty());
    SkDebugf("datas.data()=%p\n",datas.begin());
    return sk_make_sp<SkFontMgr_Custom>(DataFontLoader2(datas.begin(), static_cast<int>(datas.size())));
}
sk_sp<SkFontMgr> SkFontMgr_New_Custom_Data2(SkMemoryStream *stream) {
    return sk_make_sp<SkFontMgr_Custom>(DataFontLoader3(stream));
}
