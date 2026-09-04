cmake_minimum_required(VERSION 3.20)
project(NeutronStarSimulator VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(NSIM_USE_OPENMP "Enable OpenMP parallelization for particle physics" ON)
option(NSIM_BUILD_TESTS "Build unit tests" ON)

# ---------------------------------------------------------------------------
# Dependencies via FetchContent
# ---------------------------------------------------------------------------
include(FetchContent)

# --- GLM (header-only math library) ---
FetchContent_Declare(
  glm
  URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(glm)

# --- GLFW (window/input) ---
FetchContent_Declare(
  glfw
  URL https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# --- GLAD (OpenGL loader) ---
# Vendored glad (include/glad, include/KHR, src/glad.c) is used instead of
# the Python-based FetchContent generator to avoid fragile build-tree generation.
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include)

# --- Dear ImGui ---
FetchContent_Declare(
  imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.5.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(imgui)

# imgui upstream has no CMake target; build it from the fetched sources.
add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw)
target_compile_options(imgui PRIVATE -w)

# --- ImPlot ---
FetchContent_Declare(
  implot
  URL https://github.com/epezent/implot/archive/refs/tags/v0.16.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(implot)

# implot upstream has no CMake target; build one from the fetched sources.
add_library(implot STATIC
  ${implot_SOURCE_DIR}/implot.cpp
  ${implot_SOURCE_DIR}/implot_items.cpp
)
target_include_directories(implot PUBLIC
  ${implot_SOURCE_DIR}
)
target_link_libraries(implot PUBLIC imgui)
target_compile_options(implot PRIVATE -w)

# --- spdlog (logging) ---
FetchContent_Declare(
  spdlog
  URL https://github.com/gabime/spdlog/archive/refs/tags/v1.14.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# --- stb_image ---
FetchContent_Declare(
  stb
  URL https://github.com/nothings/stb/archive/refs/heads/master.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(stb)

# stb is header-only; provide the include path as an interface target.
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})

# --- Catch2 (tests only) ---
if(NSIM_BUILD_TESTS)
  FetchContent_Declare(
    catch2
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.7.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(catch2)
endif()

# ---------------------------------------------------------------------------
# Core library
# ---------------------------------------------------------------------------
add_library(nsim_physics
  src/Physics/Gravity.cpp
  src/Physics/Relativity.cpp
  src/Physics/Rotation.cpp
  src/Physics/TOVSolver.cpp
  src/Physics/EquationOfState.cpp
)
target_include_directories(nsim_physics PUBLIC include)
target_link_libraries(nsim_physics PUBLIC glm::glm)

add_library(nsim_simulation
  src/Simulation/ParticleSystem.cpp
  src/Simulation/AccretionDisk.cpp
  src/Simulation/NeutronStar.cpp
  src/Simulation/Simulation.cpp
)
target_include_directories(nsim_simulation PUBLIC include)
target_link_libraries(nsim_simulation PUBLIC nsim_physics)

add_library(nsim_rendering
  src/Rendering/Shader.cpp
  src/Rendering/Camera.cpp
  src/Rendering/Mesh.cpp
  src/Rendering/Renderer.cpp
  src/Rendering/Framebuffer.cpp
)
target_include_directories(nsim_rendering PUBLIC
  include
  ${glfw_SOURCE_DIR}/include
  ${spdlog_SOURCE_DIR}/include
)
target_link_libraries(nsim_rendering PUBLIC nsim_simulation glad glfw glm::glm spdlog)

add_library(nsim_ui
  src/UI/SimulationUI.cpp
)
target_include_directories(nsim_ui PUBLIC
  include
  ${imgui_SOURCE_DIR}
  ${implot_SOURCE_DIR}
)
target_link_libraries(nsim_ui PUBLIC nsim_simulation imgui implot)

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
add_executable(NeutronStarSimulator
  src/main.cpp
)
target_link_libraries(NeutronStarSimulator PUBLIC
  nsim_physics nsim_simulation nsim_rendering nsim_ui
  glfw glad glm::glm imgui implot spdlog stb_image
)

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  add_compile_definitions(NOMINMAX)
  foreach(target nsim_physics nsim_simulation nsim_rendering nsim_ui NeutronStarSimulator)
    target_compile_options(${target} PUBLIC /W4 /utf-8)
  endforeach()
else()
  foreach(target nsim_physics nsim_simulation nsim_rendering nsim_ui NeutronStarSimulator)
    target_compile_options(${target} PUBLIC -Wall -Wextra -Wpedantic)
  endforeach()
endif()

if(NSIM_USE_OPENMP)
  find_package(OpenMP)
  if(OpenMP_FOUND)
    target_link_libraries(nsim_simulation PUBLIC OpenMP::OpenMP_CXX)
    target_compile_options(nsim_simulation PUBLIC -fopenmp)
  endif()
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(NSIM_BUILD_TESTS)
  enable_testing()

  add_executable(test_gravity tests/test_gravity.cpp)
  add_executable(test_relativity tests/test_relativity.cpp)
  add_executable(test_tov tests/test_tov.cpp)

  foreach(test_target test_gravity test_relativity test_tov)
    target_include_directories(${test_target} PUBLIC
      include
      ${spdlog_SOURCE_DIR}/include
    )
    target_link_libraries(${test_target} PUBLIC nsim_physics spdlog)
  endforeach()

  add_custom_target(nsim_tests DEPENDS test_gravity test_relativity test_tov)

  add_test(NAME test_gravity COMMAND test_gravity)
  add_test(NAME test_relativity COMMAND test_relativity)
  add_test(NAME test_tov COMMAND test_tov)
  include(CTest)
endif()

# ---------------------------------------------------------------------------
# Copy assets to build directory (produces <build>/assets/shaders/...)
# ---------------------------------------------------------------------------
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
