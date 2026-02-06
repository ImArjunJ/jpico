add_library(jpico_hal INTERFACE)

target_include_directories(jpico_hal INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jpico_hal INTERFACE
    jpico_core
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_dma
)
