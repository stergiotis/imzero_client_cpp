#include "app.h"

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

int main(int argc, char** argv) {
#ifdef TRACY_ENABLE
    ImGui::SetAllocatorFunctions(imZeroMemAlloc,imZeroMemFree,nullptr);
#endif

    CliOptions opts{};
    opts.parse(argc, argv, stderr);

    App app{};
    return app.Run(opts);
}