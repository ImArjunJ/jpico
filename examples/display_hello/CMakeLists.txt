add_executable(example_display_hello main.cpp)

target_link_libraries(example_display_hello
    jpico_core
    jpico_hal
    jpico_ili9341
    jpico_graphics
    pico_stdlib
)

pico_enable_stdio_usb(example_display_hello 1)
pico_enable_stdio_uart(example_display_hello 0)
pico_add_extra_outputs(example_display_hello)
