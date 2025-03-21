#pragma once
#include <stdint.h>
#include "imgui.h"
typedef uint16_t ItemStatusE;
enum ItemStatusE_ {
    ItemStatusFocused = 1 << 0,
    ItemStatusHoveredPrimary = 1 << 1,
    ItemStatusHoveredSecondary = 1 << 2,
    ItemStatusActive = 1 << 3,
    ItemStatusEdited = 1 << 4,
    ItemStatusActivated = 1 << 5,
    ItemStatusDeactivated = 1 << 6,
    ItemStatusDeactivatedAfterEdit = 1 << 7,
    ItemStatusVisible = 1 << 8,
    ItemStatusClicked = 1 << 9,
    ItemStatusToggleOpen = 1 << 10,
};

ItemStatusE GetItemStatus(ImGuiHoveredFlags primary=ImGuiHoveredFlags_None, ImGuiHoveredFlags secondary=ImGuiHoveredFlags_ForTooltip);

bool BeginCustomWidget(ImDrawList **dlOut,ImVec2 *posOut, ImVec2 *avail,bool *keyboardNavActive,ImGuiID *seed);
