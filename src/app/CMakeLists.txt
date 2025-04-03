add_subdirectory(web_server)

add_library(app_lib INTERFACE)

target_link_libraries(app_lib INTERFACE
    web_server_lib
)
