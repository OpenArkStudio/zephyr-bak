if (UNIX)
    find_package(Threads REQUIRED)
endif ()

# client
set(client_project "client")
file(GLOB CLIENT_SRC src/client.cpp ${ROOT_DIR}/zephyr/*.hpp)
#list(FILTER ${CLIENT_SRC} EXCLUDE REGEX "*rpc*")
add_executable(${client_project} ${CLIENT_SRC})
if (UNIX)
    target_link_libraries(${client_project} pthread)
else ()
    target_link_libraries(${client_project})
endif ()


# server
set(server_project "server")
file(GLOB SERVER_SRC src/server.cpp ${ROOT_DIR}/zephyr/*.hpp)
#list(FILTER ${SERVER_SRC} EXCLUDE REGEX "*rpc*")
add_executable(${server_project} ${SERVER_SRC})
if (UNIX)
    target_link_libraries(${server_project} pthread)
else ()
    target_link_libraries(${server_project})
endif ()