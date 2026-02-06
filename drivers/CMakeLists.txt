# drivers/CMakeLists.txt — umbrella for all driver sub-modules.
# Each driver is gated behind its own option flag.

if(JPICO_ENABLE_ILI9341)
    add_subdirectory(ili9341)
endif()
