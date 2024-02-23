#include "common.h"
#include "imgui_internal.h"

ItemStatusE GetItemStatus(ImGuiHoveredFlags primary, ImGuiHoveredFlags secondary) {
    ItemStatusE r = 0;
#define SET_FLAG(check,flag) r = (check) ? (r | (flag)) : r
    SET_FLAG(ImGui::IsItemFocused(),ItemStatusFocused);
    SET_FLAG(ImGui::IsItemHovered(primary),ItemStatusHoveredPrimary);
    SET_FLAG(ImGui::IsItemHovered(secondary),ItemStatusHoveredSecondary);
    SET_FLAG(ImGui::IsItemActive(),ItemStatusActive);
    SET_FLAG(ImGui::IsItemEdited(),ItemStatusEdited);
    SET_FLAG(ImGui::IsItemActivated(),ItemStatusActivated);
    SET_FLAG(ImGui::IsItemDeactivated(),ItemStatusDeactivated);
    SET_FLAG(ImGui::IsItemDeactivatedAfterEdit(),ItemStatusDeactivatedAfterEdit);
    SET_FLAG(ImGui::IsItemVisible(),ItemStatusVisible);
    SET_FLAG(ImGui::IsItemToggledOpen(),ItemStatusToggleOpen);
    SET_FLAG(ImGui::IsItemClicked(),ItemStatusClicked);
    
    SET_FLAG((ImGui::GetItemFlags() & ImGuiItemFlags_Disabled) != 0,1 << 11);
#undef SET_FLAG
    return r;
}

bool BeginCustomWidget(ImDrawList **dlOut,ImVec2 *posOut, ImVec2 *avail, bool *keyboardNavActive,ImGuiID *seed) {
    ImGuiContext& g = *GImGui;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    *dlOut = window->DrawList;
    *posOut = window->DC.CursorPos;
    *avail = ImGui::GetContentRegionAvail();
    *keyboardNavActive = g.IO.NavVisible;
    *seed = window->IDStack.back();
    return !(window->SkipItems);
}
