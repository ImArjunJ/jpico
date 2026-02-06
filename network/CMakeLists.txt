add_library(jpico_network STATIC
    src/wifi.cpp
)

target_include_directories(jpico_network PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(NOT JPICO_NETWORK_LWIPOPTS_PATH)
    set(JPICO_NETWORK_LWIPOPTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config")
endif()

target_include_directories(jpico_network PUBLIC
    ${JPICO_NETWORK_LWIPOPTS_PATH}
)

target_link_libraries(jpico_network PUBLIC
    jpico_core
    pico_stdlib
    pico_cyw43_arch_lwip_poll
)

target_compile_definitions(jpico_network PUBLIC
    PICO_CYW43_ARCH_POLL=1
)
