# client
set(client_project "client")
file(GLOB CLIENT_SRC src/client.cpp ${ROOT_DIR}/zephyr/*.hpp)
list(FILTER ${CLIENT_SRC} EXCLUDE REGEX "*rpc*")
add_executable(${client_project} ${CLIENT_SRC})
target_link_libraries(${client_project} ${PROTOBUF_LIBRARIES})
set_target_properties(${client_project} PROPERTIES
        FOLDER "examples"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})


# server
set(server_project "server")
file(GLOB SERVER_SRC src/server.cpp ${ROOT_DIR}/zephyr/*.hpp)
list(FILTER ${SERVER_SRC} EXCLUDE REGEX "*rpc*")
add_executable(${server_project} ${SERVER_SRC})
target_link_libraries(${server_project} ${PROTOBUF_LIBRARIES})
set_target_properties(${server_project} PROPERTIES
        FOLDER "examples"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})