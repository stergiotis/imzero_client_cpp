#pragma once
#include <flatbuffers/flatbuffers.h>

class ImZeroPlayerEventProcessor {
public:
private:
    flatbuffers::FlatBufferBuilder *fbBuilder;
    std::vector<flatbuffers::Offset<VectorCmdFB::SingleVectorCmdDto>> *_FbCmds;
};