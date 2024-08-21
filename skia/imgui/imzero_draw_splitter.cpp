#include "imzero_draw_splitter.h"
#include "tracy/Tracy.hpp"
#include "imgui_internal.h"
#include "imzero_draw_utils.h"

#define ImDrawCmd_HeaderSize                            (offsetof(ImDrawCmd, VtxOffset) + sizeof(unsigned int))
#define ImDrawCmd_HeaderCompare(CMD_LHS, CMD_RHS)       (memcmp(CMD_LHS, CMD_RHS, ImDrawCmd_HeaderSize))    // Compare ClipRect, TextureId, VtxOffset
#define ImDrawCmd_HeaderCopy(CMD_DST, CMD_SRC)          (memcpy(CMD_DST, CMD_SRC, ImDrawCmd_HeaderSize))    // Copy ClipRect, TextureId, VtxOffset

void ImDrawListSplitter::ClearFreeMemory()
{ ZoneScoped;
    for (int i = 0; i < _Channels.Size; i++)
    {
        if (i == _Current) {
            memset(&_Channels[i], 0, sizeof(_Channels[i]));  // Current channel is a copy of CmdBuffer/IdxBuffer, don't destruct again
        }
        _Channels[i]._CmdBuffer.clear();
        _Channels[i]._IdxBuffer.clear();
    }
#ifdef IMZERO_DRAWLIST
    for (int i = 0; i < _ChannelsFbCmds.Size; i++)
    {
        if (i == _Current) {
            _ChannelsFbCmds[i] = nullptr;
            _ChannelsFbBuilders[i] = nullptr;
        } else {
            _ChannelsFbCmds[i]->clear();
            delete _ChannelsFbCmds[i];
            _ChannelsFbBuilders[i]->Clear();
            delete _ChannelsFbBuilders[i];
        }
    }
    _ChannelsFbCmds.clear();
    _ChannelsFbBuilders.clear();
#endif
    _Current = 0;
    _Count = 1;
    _Channels.clear();
}

void ImDrawListSplitter::Split(ImDrawList* draw_list, int channels_count)
{ ZoneScoped;
    IM_UNUSED(draw_list);
    IM_ASSERT(_Current == 0 && _Count <= 1 && "Nested channel splitting is not supported. Please use separate instances of ImDrawListSplitter.");
    int old_channels_count = _Channels.Size;
    if (old_channels_count < channels_count)
    {
        _Channels.reserve(channels_count); // Avoid over reserving since this is likely to stay stable
        _Channels.resize(channels_count);
#ifdef IMZERO_DRAWLIST
        _ChannelsFbCmds.reserve(channels_count);
        _ChannelsFbCmds.resize(channels_count);
        _ChannelsFbBuilders.reserve(channels_count);
        _ChannelsFbBuilders.resize(channels_count);
#endif
    }
    _Count = channels_count;

    // Channels[] (24/32 bytes each) hold storage that we'll swap with draw_list->_CmdBuffer/_IdxBuffer
    // The content of Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
    // When we switch to the next channel, we'll copy draw_list->_CmdBuffer/_IdxBuffer into Channels[0] and then Channels[1] into draw_list->CmdBuffer/_IdxBuffer
    memset(&_Channels[0], 0, sizeof(ImDrawChannel));
    for (int i = 1; i < channels_count; i++)
    {
        if (i >= old_channels_count)
        {
            IM_PLACEMENT_NEW(&_Channels[i]) ImDrawChannel();
#ifdef IMZERO_DRAWLIST
            _ChannelsFbCmds[i] = new std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>>();
            _ChannelsFbBuilders[i] = new flatbuffers::FlatBufferBuilder();
#endif
        }
        else
        {
            _Channels[i]._CmdBuffer.resize(0);
            _Channels[i]._IdxBuffer.resize(0);
#ifdef IMZERO_DRAWLIST
            _ChannelsFbCmds[i]->resize(0);
            _ChannelsFbBuilders[i]->Clear();
#endif
        }
    }
}

void ImDrawListSplitter::Merge(ImDrawList* draw_list)
{ ZoneScoped;
    // Note that we never use or rely on _Channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    if (_Count <= 1)
        return;

    SetCurrentChannel(draw_list, 0);
    draw_list->_PopUnusedDrawCmd();


    // Calculate our final buffer sizes. Also fix the incorrect IdxOffset values in each command.
    int new_cmd_buffer_count = 0;
    int new_idx_buffer_count = 0;
#ifdef IMZERO_DRAWLIST
    IM_ASSERT_PARANOID(draw_list->fbBuilder == _ChannelsFbBuilders[0] && "lowest channel is active channel");
    IM_ASSERT_PARANOID(draw_list->_FbCmds == _ChannelsFbCmds[0] && "lowest channel is active channel");
    int new_fb_cmds_count = 0;
#endif
    ImDrawCmd* last_cmd = (_Count > 0 && draw_list->CmdBuffer.Size > 0) ? &draw_list->CmdBuffer.back() : NULL;
    int idx_offset = last_cmd ? last_cmd->IdxOffset + last_cmd->ElemCount : 0;
    for (int i = 1; i < _Count; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (ch._CmdBuffer.Size > 0 && ch._CmdBuffer.back().ElemCount == 0 && ch._CmdBuffer.back().UserCallback == NULL) // Equivalent of PopUnusedDrawCmd()
            ch._CmdBuffer.pop_back();

        if (ch._CmdBuffer.Size > 0 && last_cmd != NULL)
        {
            // Do not include ImDrawCmd_AreSequentialIdxOffset() in the compare as we rebuild IdxOffset values ourselves.
            // Manipulating IdxOffset (e.g. by reordering draw commands like done by RenderDimmedBackgroundBehindWindow()) is not supported within a splitter.
            ImDrawCmd* next_cmd = &ch._CmdBuffer[0];
            if (ImDrawCmd_HeaderCompare(last_cmd, next_cmd) == 0 && last_cmd->UserCallback == NULL && next_cmd->UserCallback == NULL)
            {
                // Merge previous channel last draw command with current channel first draw command if matching.
                last_cmd->ElemCount += next_cmd->ElemCount;
                idx_offset += next_cmd->ElemCount;
                ch._CmdBuffer.erase(ch._CmdBuffer.Data); // FIXME-OPT: Improve for multiple merges.
            }
        }
        if (ch._CmdBuffer.Size > 0)
            last_cmd = &ch._CmdBuffer.back();
        new_cmd_buffer_count += ch._CmdBuffer.Size;
        new_idx_buffer_count += ch._IdxBuffer.Size;
#ifdef IMZERO_DRAWLIST
        new_fb_cmds_count += static_cast<int>(_ChannelsFbCmds[i]->size());
#endif
        for (int cmd_n = 0; cmd_n < ch._CmdBuffer.Size; cmd_n++)
        {
            ch._CmdBuffer.Data[cmd_n].IdxOffset = idx_offset;
            idx_offset += static_cast<int>(ch._CmdBuffer.Data[cmd_n].ElemCount);
        }
    }
    draw_list->CmdBuffer.resize(draw_list->CmdBuffer.Size + new_cmd_buffer_count);
    draw_list->IdxBuffer.resize(draw_list->IdxBuffer.Size + new_idx_buffer_count);
#ifdef IMZERO_DRAWLIST
    draw_list->_FbCmds->reserve(draw_list->_FbCmds->size() + new_fb_cmds_count);
#endif

    // Write commands and indices in order (they are fairly small structures, we don't copy vertices only indices)
    ImDrawCmd* cmd_write = draw_list->CmdBuffer.Data + draw_list->CmdBuffer.Size - new_cmd_buffer_count;
    ImDrawIdx* idx_write = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size - new_idx_buffer_count;
    for (int i = 1; i < _Count; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (int sz = ch._CmdBuffer.Size) { memcpy(cmd_write, ch._CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if (int sz = ch._IdxBuffer.Size) { memcpy(idx_write, ch._IdxBuffer.Data, sz * sizeof(ImDrawIdx)); idx_write += sz; }
#ifdef IMZERO_DRAWLIST
        { ZoneScopedN("serialize and add split drawlist");
            // build command
            auto builder = _ChannelsFbBuilders[i];
            auto dlFb = createVectorCmdFBDrawList(*draw_list,true,*_ChannelsFbCmds[i],*builder);
            builder->Finish(dlFb,nullptr);

            // serialize
            auto const buf = builder->GetBufferPointer();
            auto const bufSize = builder->GetSize();

            // append to drawlist
            auto const bufVec = draw_list->fbBuilder->CreateVector<uint8_t>(buf,bufSize);
            auto const cmd = ImZeroFB::CreateCmdWrappedDrawList(*draw_list->fbBuilder,bufVec);
            draw_list->addVectorCmdFB(ImZeroFB::VectorCmdArg_CmdWrappedDrawList,cmd.Union());

            builder->Clear();
        }
        _ChannelsFbCmds[i]->resize(0);
        _ChannelsFbBuilders[i]->Clear();
#endif
    }
    draw_list->_IdxWritePtr = idx_write;

    // Ensure there's always a non-callback draw command trailing the command-buffer
    if (draw_list->CmdBuffer.Size == 0 || draw_list->CmdBuffer.back().UserCallback != NULL)
        draw_list->AddDrawCmd();

    // If current command is used with different settings we need to add a new command
    ImDrawCmd* curr_cmd = &draw_list->CmdBuffer.Data[draw_list->CmdBuffer.Size - 1];
    if (curr_cmd->ElemCount == 0)
        ImDrawCmd_HeaderCopy(curr_cmd, &draw_list->_CmdHeader); // Copy ClipRect, TextureId, VtxOffset
    else if (ImDrawCmd_HeaderCompare(curr_cmd, &draw_list->_CmdHeader) != 0)
        draw_list->AddDrawCmd();

    _Count = 1;
}

void ImDrawListSplitter::SetCurrentChannel(ImDrawList* draw_list, int idx)
{ ZoneScoped;
    IM_ASSERT(idx >= 0 && idx < _Count);

#ifdef IMZERO_DRAWLIST
    // _Current is zero when ImDrawListSplitter gets initialized but _Channels.Data[0] is not properly
    // initialized. Why does the regular ImGui code get aways with the check below?
    if(_ChannelsFbCmds.size() < idx) { ZoneScopedN("initialize additional channels");
        auto b = _ChannelsFbCmds.size();
        _ChannelsFbCmds.reserve(idx);
        _ChannelsFbCmds.resize(idx);
        _ChannelsFbBuilders.reserve(idx);
        _ChannelsFbBuilders.resize(idx);
        IM_ASSERT(b > 0 && "first slot is reserved for drawlist's objects");
        for(auto i=b;i<idx;i++) {
           _ChannelsFbCmds[i] = new std::vector<flatbuffers::Offset<ImZeroFB::SingleVectorCmdDto>>();
           _ChannelsFbBuilders[i] = new flatbuffers::FlatBufferBuilder();
        }
    }
#endif

    // NOTE: this function needs to be idempotent
    if (_Current == idx)
        return;


    // Overwrite ImVector (12/16 bytes), four times. This is merely a silly optimization instead of doing .swap()
    memcpy(&_Channels.Data[_Current]._CmdBuffer, &draw_list->CmdBuffer, sizeof(draw_list->CmdBuffer));
    memcpy(&_Channels.Data[_Current]._IdxBuffer, &draw_list->IdxBuffer, sizeof(draw_list->IdxBuffer));
#ifdef IMZERO_DRAWLIST
    IM_ASSERT(_ChannelsFbCmds[idx] != nullptr && "uninitialized channel fb");
    _ChannelsFbCmds[_Current] = draw_list->_FbCmds;
    _ChannelsFbBuilders[_Current] = draw_list->fbBuilder;
#endif
    _Current = idx;
    memcpy(&draw_list->CmdBuffer, &_Channels.Data[idx]._CmdBuffer, sizeof(draw_list->CmdBuffer));
    memcpy(&draw_list->IdxBuffer, &_Channels.Data[idx]._IdxBuffer, sizeof(draw_list->IdxBuffer));
    draw_list->_IdxWritePtr = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size;
#ifdef IMZERO_DRAWLIST
    IM_ASSERT(_ChannelsFbCmds[idx] != nullptr && "uninitialized channel fb");
    draw_list->_FbCmds = _ChannelsFbCmds[idx];
    draw_list->fbBuilder = _ChannelsFbBuilders[idx];
#endif

    // If current command is used with different settings we need to add a new command
    ImDrawCmd* curr_cmd = (draw_list->CmdBuffer.Size == 0) ? NULL : &draw_list->CmdBuffer.Data[draw_list->CmdBuffer.Size - 1];
    if (curr_cmd == NULL)
        draw_list->AddDrawCmd();
    else if (curr_cmd->ElemCount == 0)
        ImDrawCmd_HeaderCopy(curr_cmd, &draw_list->_CmdHeader); // Copy ClipRect, TextureId, VtxOffset
    else if (ImDrawCmd_HeaderCompare(curr_cmd, &draw_list->_CmdHeader) != 0)
        draw_list->AddDrawCmd();
}
