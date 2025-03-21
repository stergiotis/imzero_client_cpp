#pragma once

#include "imgui.h"
#include <flatbuffers/flatbuffers.h>

namespace ImGuiSkia {
  void fbAddPointsToVector(
      flatbuffers::Offset<flatbuffers::Vector<float>> &xs, flatbuffers::Offset<flatbuffers::Vector<float>> &ys,
      flatbuffers::FlatBufferBuilder &builder, const ImVec2 *points, const int points_count);



  // see https://github.com/google/flatbuffers/issues/4463
  template <typename Type>
  Type* GetMutablePointer(flatbuffers::FlatBufferBuilder& builder, const flatbuffers::Offset<Type>& object) {
    return (reinterpret_cast<Type *>(builder.GetCurrentBufferPointer() + builder.GetSize() - object.o));
  }

  // see https://github.com/google/flatbuffers/issues/4463
  template <typename Type>
  Type* GetPointer(flatbuffers::FlatBufferBuilder& builder, flatbuffers::Offset<Type>& object) {
    return (reinterpret_cast<Type *>(builder.GetCurrentBufferPointer() + builder.GetSize() - object.o));
  }

  // see https://github.com/google/flatbuffers/issues/4463
  template <typename Type>
  const Type* GetPointer(flatbuffers::FlatBufferBuilder& builder, const flatbuffers::Offset<Type>& object) {
    return (reinterpret_cast<const Type *>(builder.GetCurrentBufferPointer() + builder.GetSize() - object.o));
  }
}