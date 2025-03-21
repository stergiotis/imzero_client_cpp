#include "imgui_skia_tracy_support.h"

#include "imgui.h"
#include <cstdlib>
#include "tracy/Tracy.hpp"

static const char *tracyMemPoolNameImgui = "imgui";
static void *imguiSkiaMemAlloc(size_t sz,void *user_data) noexcept { ZoneScoped;
    auto ptr = malloc(sz);
#ifdef IMGUI_SKIA_DEBUG_BUILD
    TracyAllocNS(ptr, sz, 6, tracyMemPoolNameImgui);
#else
    TracyAllocN(ptr, sz, tracyMemPoolNameImgui);
#endif
    return ptr;
}
static void imguiSkiaMemFree(void *ptr,void *user_data) noexcept { ZoneScoped;
#ifdef IMGUI_SKIA_DEBUG_BUILD
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

void ImGuiSkia::SetupImGuiSkiaTracySupportEnabled() {
    ImGui::SetAllocatorFunctions(imguiSkiaMemAlloc,imguiSkiaMemFree,nullptr);
}