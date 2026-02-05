cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion RISCV_ZCB_RPI_2_2_0_3)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(jpico C CXX ASM)

pico_sdk_init()


set(PROJECT_SOURCES
    src/main.cpp
    src/drivers/ili9341.cpp
    src/graphics/canvas.cpp
    src/network/wifi.cpp
)

add_executable(jpico ${PROJECT_SOURCES})

pico_set_program_name(jpico "jpico")
pico_set_program_version(jpico "0.1")

pico_enable_stdio_uart(jpico 0)
pico_enable_stdio_usb(jpico 1)

target_include_directories(jpico PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/config
)

target_link_libraries(jpico
    pico_stdlib
    hardware_spi
    pico_cyw43_arch_lwip_poll
)

target_compile_definitions(jpico PRIVATE
    PICO_CYW43_ARCH_POLL=1
)

pico_add_extra_outputs(jpico)
