cmake_minimum_required(VERSION 3.14)
project(UPMEM_Project C)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build host code
add_subdirectory(host)

# Add custom target for DPU code
add_custom_target(dpu_code ALL
    COMMAND dpu-upmem-dpurte-clang -O2 -o ${CMAKE_BINARY_DIR}/bin/dpu_code dpu/task.c -I ${CMAKE_SOURCE_DIR}/support/
    DEPENDS ${CMAKE_SOURCE_DIR}/dpu/task.c ${CMAKE_SOURCE_DIR}/support/common.h
    COMMENT "Building DPU binary..."
)

