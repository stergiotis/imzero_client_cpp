#include "imzero_draw_font.h"
#include "tracy/Tracy.hpp"
#include "imzero_draw_utils.h"
#include "imgui_internal.h"

static char hiddenPwBuffer[512];
static size_t hiddenPwBufferNChars = 0;
static size_t hiddenPwBufferNBytesPerChar = 0;
