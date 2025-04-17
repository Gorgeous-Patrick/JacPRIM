# Add include directories
include_directories(/opt/upmem/include /opt/upmem/include/dpu ${CMAKE_SOURCE_DIR}/support/)

# Add library directories
link_directories(/opt/upmem/lib)

# Host C code using normal compiler
add_executable(host app.c)

# Link UPMEM host runtime
target_link_libraries(host
    dpu
    pthread
)

