#define IM_VEC2_CLASS_EXTRA                                                     \
        constexpr ImVec2(const float *f) : x(f[0]), y(f[1]) {}

#define IM_VEC4_CLASS_EXTRA                                                     \
        constexpr ImVec4(const float *f) : x(f[0]), y(f[1]), z(f[2]), w(f[3]) {}

#define IMPLOT_POINT_CLASS_EXTRA                                                \
        constexpr ImPlotPoint(const double *d) : x(d[0]), y(d[1]) {}
