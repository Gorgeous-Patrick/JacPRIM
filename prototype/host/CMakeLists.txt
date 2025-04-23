# Set host compiler (override if inherited from DPU subdir)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Host executable
add_executable(host app.cpp graph/generator.cpp graph/graph_cut.cpp)

# Link UPMEM SDK libraries
target_include_directories(host PRIVATE /opt/upmem/include/dpu)
target_include_directories(host PRIVATE ../support)
target_link_directories(host PRIVATE /opt/upmem/lib)
target_link_libraries(host PRIVATE dpu)
