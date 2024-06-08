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

CliOptions opts;

Application* Application::Create(int argc, char** argv, void* platformData) {
    #ifdef TRACY_ENABLE
    ImGui::SetAllocatorFunctions(imZeroMemAlloc,imZeroMemFree,nullptr);
    #endif

    opts.parse(argc,argv,stderr);
    ImGui::skiaFontDyFudge = opts.fontDyFudge;

    if(opts.fffiInterpreter) {
        if(opts.fffiInFile != nullptr) {
            fdIn = fopen(opts.fffiInFile, "r");
            if(fdIn == nullptr) {
                fprintf(stderr, "unable to open fffInFile %s: %s", opts.fffiInFile, strerror(errno));
                exit(1);
            }
            setvbuf(fdIn, nullptr,_IONBF,0);
        }
        if(opts.fffiOutFile != nullptr) {
            fdOut = fopen(opts.fffiOutFile, "w");
            if(fdOut == nullptr) {
                fprintf(stderr, "unable to open fffOutFile %s: %s", opts.fffiOutFile, strerror(errno));
                exit(1);
            }
            setvbuf(fdOut, nullptr,_IONBF,0);
        }
    }

    auto ttfData = SkData::MakeFromFileName(opts.ttfFilePath);
    auto fontMgr = SkFontMgr_New_Custom_Data(SkSpan<sk_sp<SkData>>(&ttfData,1));

    const auto typeface = fontMgr->makeFromData(ttfData);
    //const auto typeface = fontMgr->matchFamilyStyle(nullptr,SkFontStyle());

    assert(typeface != nullptr);
    assert(fontMgr->countFamilies() > 0);
    ImGui::paragraph = std::make_shared<Paragraph>(fontMgr,typeface);

    ImGui::skiaFont = SkFont(typeface);

    return new ImZeroSkiaClient(argc, argv, platformData);
}

ImZeroSkiaClient::ImZeroSkiaClient(int argc, char** argv, void* platformData) : fBackendType(Window::kRaster_BackendType) {
    if(strcmp(opts.skiaBackendType,"gl") == 0) {
#if defined(SK_GL)
        fBackendType=Window::kNativeGL_BackendType;
#else
        fprintf(stderr,"gl backend is not supported (SK_GL not defined)\n");
        exit(1);
#endif
    }
    if(strcmp(opts.skiaBackendType,"vulkan") == 0) {
#if defined(SK_VULKAN)
        fBackendType=Window::kVulkan_BackendType;
#else
        fprintf(stderr,"vulkan backend is not supported (SK_VULKAN not defined)\n");
        exit(1);
#endif
    }
    if(strcmp(opts.skiaBackendType,"raster") == 0) {
        fBackendType=Window::kRaster_BackendType;
    }

    SkGraphics::Init();

    fWindow = Window::CreateNativeWindow(platformData);
    {
        auto params = DisplayParams();
        params.fDisableVsync = !opts.vsync;
        fWindow->setRequestedDisplayParams(params);
    }

    fImGuiLayer = std::make_unique<ImGuiLayer>(&opts);
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

    fWindow->setTitle(opts.appTitle);
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

