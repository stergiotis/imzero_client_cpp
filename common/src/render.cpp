#include "render.h"
#include <cstddef>
#include <cstdarg>

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>

#include <cstdint>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include "marshalling/casts.h"
#include <imgui.h>
#include <implot.h>
#include <TextEditor.h>

#include "widgets/common.h"
#include "widgets/piemenu.h"
#include "widgets/splitter.h"
#include "widgets/imgui_knobs/imgui-knobs.h"
#include "widgets/imgui_toggle/imgui_toggle.h"
#include "widgets/imgui_implot/implot.h"
#include "widgets/imgui_implot/implot_internal.h"
#include "widgets/imgui_coolbar/ImCoolbar.h"
#include "widgets/imgui_flamegraph/imgui_widget_flamegraph.h"
#include "widgets/imgui_flamegraph/adapter.h"
#include "widgets/imgui_club/hexeditor.h"
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#endif
#define IMSPINNER_DEMO
#include "widgets/imgui_imspinner/imspinner.h"
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop 5
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include "arena/simple/simple.h"
#include "marshalling/receive.h"
#include "marshalling/send.h"
#include "marshalling/helper.h"

const uint32_t FuncProcIdFlush = 0xffffffff;
#ifdef TRACY_ENABLE
#include "tracy/Tracy.hpp"
#define FFFI_FUNCTION_INVOCATION(funcName) ZoneScopedN(funcName);
#define FFFI_FUNCTION_FLUSH() ZoneScopedN("fffi flush");
#else
#if 1
#define FFFI_FUNCTION_INVOCATION(funcName)
#define FFFI_FUNCTION_FLUSH()
#else
#define FFFI_FUNCTION_INVOCATION(funcName) fprintf(stderr, "invoking %s\n", funcName);
#define FFFI_FUNCTION_FLUSH() fprintf(stderr,"flushing\n");
#endif
#endif

uint32_t lap = 0;
void interpretCommands() {
    lap++;
    arenaReset((lap & 0xff) == 0);
    bool flushed = false;
    while(!flushed) {
        const auto funcId = receiveValue<uint32_t>();
        switch(funcId) { // this should be reordered by pgo
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif
#include "binding/imgui/dispatch.h"
#include "binding/implot/dispatch.h"
#include "binding/imcolortextedit/dispatch.h"
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
            case FuncProcIdFlush:
                FFFI_FUNCTION_FLUSH()
                flushed = true;
                break;
        }
    }
}

/*static void setup_imgui() {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
}*/

#include "implot_style.h"
#include "adobe_spectrum_style.h"
void render_init(FILE *fdInput,FILE *fdOutput) {
    fdIn = fdInput;
    fdOut = fdOutput;
    //StyleImPlot();
    //StyleAdobeSpectrum();
    ImGui::StyleColorsDark();
    //setup_imgui();

    ImPlot::CreateContext();

    arenaInit();
    sendInit();
    receiveInit();

    interpretCommands();
    fprintf(stderr,"end of render_init\n");
}


void render_render() {
    interpretCommands();
    //EmitDrawList(stdout);
}

void render_cleanup() {
	ImPlot::DestroyContext();
}
