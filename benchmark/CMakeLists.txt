function(BENCHMARK_FUNCTION project_name)
    file(GLOB_RECURSE BENCHMARK_SRC ${project_name}.cpp)
    add_executable(${project_name} ${BENCHMARK_SRC})
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
    set_target_properties(${project_name} PROPERTIES
            FOLDER "benchmark"
            ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
            RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
            LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
endfunction()

set(BENCHMARKS
        ping_pong_client
        ping_pong_server
        broadcast_client
        broadcast_server)

foreach(benchmark_index ${BENCHMARKS})
    BENCHMARK_FUNCTION(${benchmark_index})
endforeach()