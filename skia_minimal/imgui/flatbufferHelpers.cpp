#include "flatbufferHelpers.h"

void fbAddPointsToVector(
    flatbuffers::Offset<flatbuffers::Vector<float>> &xs, flatbuffers::Offset<flatbuffers::Vector<float>> &ys,
    flatbuffers::FlatBufferBuilder &builder, const ImVec2 *points, const int points_count) {
    // FIXME use fbBuilder.StartVector and EndVector to eliminate the lambda
    xs = builder.CreateVector<float>(size_t(points_count),[&points](size_t i) -> float { return points[i].x; });
    ys = builder.CreateVector<float>(size_t(points_count),[&points](size_t i) -> float { return points[i].y; });
    return;
}