add_executable(example_oled_hello main.cpp)

target_link_libraries(example_oled_hello
    jpico_core
    jpico_hal
    jpico_ssd1306
    jpico_graphics
)

pico_enable_stdio_usb(example_oled_hello 1)
pico_enable_stdio_uart(example_oled_hello 0)
pico_add_extra_outputs(example_oled_hello)
