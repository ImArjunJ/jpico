add_library(jpico_ssd1306 STATIC
    src/ssd1306.cpp
)

target_include_directories(jpico_ssd1306 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jpico_ssd1306 PUBLIC
    jpico_core
    jpico_hal
)
