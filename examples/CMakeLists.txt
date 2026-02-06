add_subdirectory(blink)

if(JPICO_ENABLE_ILI9341 AND JPICO_ENABLE_GRAPHICS)
    add_subdirectory(display_hello)
endif()

if(JPICO_ENABLE_SSD1306 AND JPICO_ENABLE_GRAPHICS)
    add_subdirectory(oled_hello)
endif()

if(JPICO_ENABLE_NETWORK)
    add_subdirectory(wifi_connect)
endif()
