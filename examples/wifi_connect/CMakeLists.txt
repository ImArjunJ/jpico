add_executable(example_wifi_connect main.cpp)

target_link_libraries(example_wifi_connect
    jpico_core
    jpico_hal
    jpico_network
)

pico_enable_stdio_usb(example_wifi_connect 1)
pico_enable_stdio_uart(example_wifi_connect 0)
pico_add_extra_outputs(example_wifi_connect)
