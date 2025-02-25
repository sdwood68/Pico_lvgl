add_library(sh1106 INTERFACE)

target_sources(sh1106 INTERFACE
    sh1106.c
)

target_include_directories(sh1106 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(sh1106 INTERFACE
    hardware_i2c
)
