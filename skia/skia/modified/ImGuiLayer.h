/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#ifndef ImGuiLayer_DEFINED
#define ImGuiLayer_DEFINED

#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint.h"
#include "include/core/SkScalar.h"
#include "include/core/SkTypes.h"
#include "include/private/base/SkTArray.h"
#include "include/private/base/SkTPin.h"
#include "include/private/base/SkTemplates.h"
#include "tools/sk_app/Window.h"
#include "include/utils/SkEventTracer.h"
//#include "tools/trace/EventTracingPriv.h"

#include <algorithm>
#include <functional>

#include "imgui.h"

#include "../vectorCmdSkiaRenderer.h"
#include "../setupUI.h"
#include "cliOptions.h"

class SkCanvas;
class SkSurface;

namespace skui {
enum class InputState;
enum class Key;
enum class ModifierKey;
}  // namespace skui

class ImGuiLayer : public sk_app::Window::Layer {
public:
    ImGuiLayer(const CliOptions *opts);
    ~ImGuiLayer() override;

    void setScaleFactor(float scaleFactor);

    typedef std::function<void(SkCanvas*)> SkiaWidgetFunc;

    void onAttach(sk_app::Window* window) override;
    void onPrePaint() override;
    void onPaint(SkSurface*) override;
    bool onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers) override;
    bool onMouseWheel(float delta, int x, int y, skui::ModifierKey modifiers) override;
    bool onKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;

private:
    sk_app::Window* fWindow;
    SkPaint fFontPaint;

    VectorCmdSkiaRenderer fVectorCmdSkiaRenderer;
    size_t fTotalVectorCmdSerializedSize{};
    size_t fSkpBytesWritten;
    size_t fSvgBytesWritten;
    size_t fPngBytesWritten;
    ImZeroSkiaSetupUI fImZeroSkiaSetupUi;
    bool ffffiInterpreter;
    SkColor fBackground;

    void drawImGuiVectorCmdsFB(SkCanvas &canvas);
    bool fUseVectorCmd;
};

#endif
