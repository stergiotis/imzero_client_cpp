#pragma once

// enable runtime-dispatch for sketch-mode rendering, disable for maximum performance
#define RENDER_MODE_SKETCH_ENABLED
// enable runtime-dispatch for svg rendering, disable for maximum performance
#define RENDER_MODE_SVG_ENABLED
// enable runtime-dispatch for backdrop filter, disable for maximum performance
#define RENDER_MODE_BACKDROP_FILTER_ENABLED

// draw red (non-intersecting) and green (intersecting) rectangles to indicate clipping rectangles
//#define SKIA_DRAW_BACKEND_DEBUG_CLIPPING