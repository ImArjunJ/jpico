add_library(jpico_graphics INTERFACE)

target_include_directories(jpico_graphics INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jpico_graphics INTERFACE
    jpico_core
)
