# CMakeList.txt : CMake project for blocks, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("blocks")

# Add source to this project's executable.
add_executable (
	"blocks"
	"src/main.c"
	"src/engine/display.h" "src/engine/display.c"
	"src/engine/cube.h" "src/engine/cube.c"
	"src/engine/player.h" "src/engine/player.c"
	"src/engine/world.h" "src/engine/world.c"
	"src/engine/types.h"
	"src/engine/window.c" "src/engine/window.h"
	"src/engine/userinputs.h" "src/engine/userinputs.c"
	"src/engine/constants.h"
	"src/engine/viewport.h" "src/engine/viewport.c"
	"src/engine/gametime.h" "src/engine/gametime.c"
	"src/engine/forces.h" "src/engine/forces.c"
	"src/engine/frustum.h" "src/engine/frustum.c"
  )

target_include_directories("blocks" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-2.1.0/include" "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.4/include" "${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglut/include")
target_link_libraries("blocks" opengl32 glu32 "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.4/lib/glfw3.lib" "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-2.1.0/lib/Release/x64/glew32.lib" "${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglut/lib/x64/freeglut.lib")

add_custom_command(
    TARGET "blocks" POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-2.1.0/bin/Release/x64/glew32.dll"
            "$<TARGET_FILE_DIR:blocks>/glew32.dll"
)

add_custom_command(
    TARGET "blocks" POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglut/bin/x64/freeglut.dll"
            "$<TARGET_FILE_DIR:blocks>/freeglut.dll"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET blocks PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
