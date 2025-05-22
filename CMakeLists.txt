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

find_package(OpenGL REQUIRED)
find_package(GLU REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)

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

target_include_directories("blocks" PRIVATE) # System headers should be found now
target_link_libraries("blocks" OpenGL::GL ${GLU_LIBRARY} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET blocks PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
