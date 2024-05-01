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
add_executable ("blocks" src/main src/window src/display src/cube src/player)

target_include_directories("blocks" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-2.1.0/include" "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.4/include")
target_link_libraries("blocks" opengl32 glu32 "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.4/lib/glfw3.lib" "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-2.1.0/lib/Release/Win32/glew32.lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET blocks PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
