/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#ifndef App_DEFINED
#define App_DEFINED

#include "include/core/SkScalar.h"
#include "include/core/SkTypes.h"
#include "tools/sk_app/Application.h"
#include "tools/sk_app/Window.h"
#include "tools/skui/ModifierKey.h"

#include "ImGuiLayer.h"

class SkSurface;

class ImZeroSkiaClient : public sk_app::Application, sk_app::Window::Layer {
public:
    ImZeroSkiaClient(int argc, char** argv, void* platformData);
    ~ImZeroSkiaClient() override;

    void onIdle() override;

    void onBackendCreated() override;
    void onPaint(SkSurface*) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;

private:
    void updateTitle();

    sk_app::Window* fWindow;
    sk_app::Window::BackendType fBackendType;

    std::unique_ptr<ImGuiLayer> fImGuiLayer;
};

#endif
