/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#include "app.h"

#include "include/core/SkGraphics.h"
#include "include/ports/SkFontMgr_data.h"
#include "include/core/SkSpan.h"

#include <cstring>

#include "vectorCmdSkiaRenderer.h"
#include "imgui.h"
#include "marshalling/receive.h"
#include "marshalling/send.h"

using namespace sk_app;
using skwindow::DisplayParams;

#ifdef TRACY_ENABLE
#include <cstdlib>
#include "tracy/Tracy.hpp"
static const char *tracyMemPoolNameImgui = "imgui";
static void *imZeroMemAlloc(size_t sz,void *user_data) noexcept { ZoneScoped;
    
    auto ptr = malloc(sz);
#ifdef IMZERO_DEBUG_BUILD
    TracyAllocNS(ptr, sz, 6, tracyMemPoolNameImgui);
#else
    TracyAllocN(ptr, sz, tracyMemPoolNameImgui);
#endif
    return ptr;
}
static void imZeroMemFree(void *ptr,void *user_data) noexcept { ZoneScoped;
#ifdef IMZERO_DEBUG_BUILD
    TracyFreeNS(ptr, 6, tracyMemPoolNameImgui);
#else
    TracyFreeN(ptr, tracyMemPoolNameImgui);
#endif
    free(ptr);
}
#if 0
// NOTE: not compatible with address sanitizier asan
static const char *tracyMemPoolNameOperators = "operator new/delete";
void* operator new(std::size_t count) {
    auto ptr = malloc(count);
    TracyAllocN(ptr, count, tracyMemPoolNameOperators);
    return ptr;
}
void operator delete(void* ptr) noexcept {
    free(ptr);
    TracyFreeN(ptr, tracyMemPoolNameOperators);
}
#endif
#endif

static bool hasFlag(int argc, char **argv,const char *flag) {
    for(int i=0;i<argc;i++) {
        if(strcmp(argv[i],flag) == 0) {
            return true;
        }
    }
    return false;
}
static const char *findFlagValueDefault(int argc, char **argv,const char *flag,const char *defaultValue) {
    for(int i=0;i<argc;i++) {
        if(strcmp(argv[i],flag) == 0) {
            if(i == argc) {
                return defaultValue;
            } else {
                return argv[i+1];
            }
        }
    }
    return defaultValue;
}
static void showUsage(const char *name, FILE *file) {
   fprintf(stderr, "%s -ttfFilePath <file.ttf> -fontDyFudge <float> -fffiInFile <fffiIn> -fffiOutFile <fffiOut> -disableVsync -disableFffi\n", name);
}

Application* Application::Create(int argc, char** argv, void* platformData) {
    #ifdef TRACY_ENABLE
    ImGui::SetAllocatorFunctions(imZeroMemAlloc,imZeroMemFree,nullptr);
    #endif

    auto const ttfFilePath = findFlagValueDefault(argc,argv,"-ttfFilePath","./SauceCodeProNerdFontPropo-Regular.ttf");
    auto const fffiInFile = findFlagValueDefault(argc, argv, "-fffiInFile", nullptr);
    auto const fffiOutFile = findFlagValueDefault(argc, argv, "-fffiOutFile", nullptr);
    auto const fontDyFudge = findFlagValueDefault(argc, argv, "-fontDyFudge", "0.0");

    {
        auto const fontDyFudgeNum = strtof(fontDyFudge, nullptr);
        if(!isnanf(fontDyFudgeNum) && fontDyFudgeNum >= -10000.0f && fontDyFudgeNum <= 10000.0f) {
            fprintf(stderr,"using font dy fudge value %f px to modify font baseline\n", fontDyFudgeNum);
            ImGui::skiaFontDyFudge = fontDyFudgeNum;
        }
    }

    if(hasFlag(argc,argv,"-help")) {
        showUsage(argv[0],stderr);
        exit(0);
    }

    const auto standalone = hasFlag(argc, argv, "-disableFffi");
    if(!standalone) {
        if(fffiInFile != nullptr) {
            fdIn = fopen(fffiInFile, "r");
            if(fdIn == nullptr) {
                fprintf(stderr, "unable to open fffInFile %s: %s", fffiInFile, strerror(errno));
                exit(1);
            }
            setvbuf(fdIn, nullptr,_IONBF,0);
        }
        if(fffiOutFile != nullptr) {
            fdOut = fopen(fffiOutFile, "w");
            if(fdOut == nullptr) {
                fprintf(stderr, "unable to open fffOutFile %s: %s", fffiOutFile, strerror(errno));
                exit(1);
            }
            setvbuf(fdOut, nullptr,_IONBF,0);
        }
    }

    auto ttfData = SkData::MakeFromFileName(ttfFilePath);
    auto fontMgr = SkFontMgr_New_Custom_Data(SkSpan<sk_sp<SkData>>(&ttfData,1));

    const auto typeface = fontMgr->makeFromData(ttfData);
    //const auto typeface = fontMgr->matchFamilyStyle(nullptr,SkFontStyle());

    assert(typeface != nullptr);
    assert(fontMgr->countFamilies() > 0);
    ImGui::paragraph = std::make_shared<Paragraph>(fontMgr,typeface);

    ImGui::skiaFont = SkFont(typeface);

    return new ImZeroSkiaClient(argc, argv, platformData);
}

ImZeroSkiaClient::ImZeroSkiaClient(int argc, char** argv, void* platformData)
#if defined(SK_GL)
        : fBackendType(Window::kNativeGL_BackendType)
#elif defined(SK_VULKAN)
        : fBackendType(Window::kVulkan_BackendType)
#else
        : fBackendType(Window::kRaster_BackendType)
#endif
        {
    const auto standalone = hasFlag(argc, argv, "-disableFffi");

    SkGraphics::Init();

    fWindow = Window::CreateNativeWindow(platformData);
    {
        auto params = DisplayParams();

        if(hasFlag(argc,argv,"-disableVsync")) {
            params.fDisableVsync = true;
        }
        fWindow->setRequestedDisplayParams(params);
    }

    fImGuiLayer = std::make_unique<ImGuiLayer>(standalone);
    fImGuiLayer->setScaleFactor(fWindow->scaleFactor());

    // register callbacks
    fWindow->pushLayer(this);
    fWindow->pushLayer(fImGuiLayer.get());

    fWindow->attach(fBackendType);
}

ImZeroSkiaClient::~ImZeroSkiaClient() {
    fWindow->detach();
    delete fWindow;
}

void ImZeroSkiaClient::updateTitle() {
    if (!fWindow) {
        return;
    }

    SkString title("ImZeroSkiaClient ");
    if (Window::kRaster_BackendType == fBackendType) {
        title.append("Raster");
    } else {
#if defined(SK_GL)
        title.append("GL");
#elif defined(SK_VULKAN)
        title.append("Vulkan");
#elif defined(SK_DAWN)
        title.append("Dawn");
#else
        title.append("Unknown GPU backend");
#endif
    }

    fWindow->setTitle(title.c_str());
}

void ImZeroSkiaClient::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
}

void ImZeroSkiaClient::onPaint(SkSurface* surface) {
}

void ImZeroSkiaClient::onIdle() {
    // Just re-paint continuously
    fWindow->inval();
}

bool ImZeroSkiaClient::onChar(SkUnichar c, skui::ModifierKey modifiers) {
/*    if (' ' == c) {
        if (Window::kRaster_BackendType == fBackendType) {
#if defined(SK_GL)
            fBackendType = Window::kNativeGL_BackendType;
#elif defined(SK_VULKAN)
            fBackendType = Window::kVulkan_BackendType;
#else
            SkDebugf("No GPU backend configured\n");
            return true;
#endif
        } else {
            fBackendType = Window::kRaster_BackendType;
        }
        fWindow->detach();
        fWindow->attach(fBackendType);
    }*/
    return true;
}

