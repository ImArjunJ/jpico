add_executable(example_blink main.cpp)

target_link_libraries(example_blink
    jpico_core
    jpico_hal
    pico_stdlib
)

pico_enable_stdio_usb(example_blink 1)
pico_enable_stdio_uart(example_blink 0)
pico_add_extra_outputs(example_blink)
