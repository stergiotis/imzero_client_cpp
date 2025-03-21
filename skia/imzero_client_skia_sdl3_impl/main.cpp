#include <SDL3/SDL_main.h>
#include "imzero_client_skia_sdl3_app.h"
#include "imzero_client_skia_sdl3_cli_options.h"

#if defined(linux) || defined(__linux) || defined(__linux__)
#include <sys/prctl.h>
#endif
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
extern "C" {
#if defined(linux) || defined(__linux) || defined(__linux__)
  int main(int argc, char* argv[]) {
#else
  int SDL_main(int argc, char* argv[]) {
#ifdef _DEBUG
static_assert(_DEBUG == 0 && "_DEBUG is not 0");
#endif
#ifdef MD_DynamicRelease
static_assert(MD_DynamicRelease == 0 && "MD_DynamicRelease is not 0");
#endif
#ifdef _DLL
static_assert(_DLL == 0 && "_DLL is not 0");
#endif
static_assert(_HAS_ITERATOR_DEBUGGING == 0 && "_HAS_ITERATOR_DEBUGGING is not 0");
static_assert(_ITERATOR_DEBUG_LEVEL == 0 && "_ITERATOR_DEBUG_LEVEL is not 0");
#endif

#ifdef TRACY_ENABLE
      ImGui::SetAllocatorFunctions(imZeroMemAlloc,imZeroMemFree,nullptr);
#endif

      ImZeroCliOptions opts{};
      uint64_t usedFlags = 0;
      if (opts.hasHelpFlag(argc, const_cast<const char **>(argv))) {
         opts.usage(argv[0],stderr);
         return 0;
      }
      opts.parse(argc, const_cast<const char**>(argv), stderr, usedFlags);
      opts.checkConsistency(argc, const_cast<const char**>(argv), stderr, usedFlags);

#if defined(linux) || defined(__linux) || defined(__linux__)
      if (0 > prctl(PR_SET_DUMPABLE, opts.fCoreDump ? 1 : 0)) {
    perror("unable to set prctl(PR_SET_DUMPABLE)");
    return 1;
      }
#endif

      ImZeroClient::App app{};
      app.setup(opts);
      app.completeFontSetup();

      const int r = app.mainLoop();
      app.cleanup();
      return r;
  }
}
