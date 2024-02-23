#!/bin/bash
set -e
rm -f main.cpp
touch main.cpp
echo "#include \"imconfig.h\"" >> main.cpp
echo "#include \"src/marshalling/casts.h\"" >> main.cpp
echo "#include \"render.h\"" >> main.cpp
cat "$1" | \
          sed 's,// Main loop,render_init();\n//Main loop,' | \
          sed 's,ImGui::NewFrame();,ImGui::NewFrame();\nrender_render();\n,' | \
          sed 's,// Cleanup,// Cleanup\nrender_cleanup();\n,' | \
          sed 's,glfwPollEvents();,glfwPostEmptyEvent();\nglfwPollEvents();,' \
          >> main.cpp
