add_executable(example_touch_paint main.cpp)

target_link_libraries(example_touch_paint
    jpico_core
    jpico_hal
    jpico_ili9341
    jpico_xpt2046
    jpico_graphics
)

pico_enable_stdio_usb(example_touch_paint 1)
pico_enable_stdio_uart(example_touch_paint 0)
pico_add_extra_outputs(example_touch_paint)
