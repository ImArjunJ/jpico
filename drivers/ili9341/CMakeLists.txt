add_library(jpico_ili9341 STATIC
    src/ili9341.cpp
)

target_include_directories(jpico_ili9341 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jpico_ili9341 PUBLIC
    jpico_core
    jpico_hal
)
