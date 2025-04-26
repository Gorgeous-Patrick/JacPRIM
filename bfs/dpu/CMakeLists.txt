# Set Clang with UPMEM target
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_FLAGS "--target=dpu-upmem-dpurte -D__UPMEM__ -O2")

# Add the DPU executable
add_executable(dpu_program task.c)

target_include_directories(dpu_program PRIVATE ../support)

# Set the output name to include `.dpu`
set_target_properties(dpu_program PROPERTIES OUTPUT_NAME "dev")
