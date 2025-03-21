#pragma once
#include "imgui_memory_editor.h"

struct HexEditor {
   MemoryEditor *memEditor;
   void *data;
   size_t data_length;

   HexEditor();
   ~HexEditor();
};
HexEditor::HexEditor() {
   this->memEditor = new MemoryEditor();
   this->data = nullptr;
   this->data_length = 0;
}
HexEditor::~HexEditor() {
   delete memEditor;
}
