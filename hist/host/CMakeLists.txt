# Host C code using normal compiler
add_executable(host app.c)

# Link UPMEM host runtime
target_link_libraries(host
    dpu
    pthread
)

