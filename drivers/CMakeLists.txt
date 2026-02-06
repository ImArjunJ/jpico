if(JPICO_ENABLE_ILI9341)
    add_subdirectory(ili9341)
endif()

if(JPICO_ENABLE_SSD1306)
    add_subdirectory(ssd1306)
endif()

if(JPICO_ENABLE_XPT2046)
    add_subdirectory(xpt2046)
endif()
