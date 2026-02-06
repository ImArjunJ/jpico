cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
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
endif()

option(JPICO_ENABLE_GRAPHICS  "Build the graphics module"     ON)
option(JPICO_ENABLE_NETWORK   "Build the network module"      ON)
option(JPICO_ENABLE_ILI9341   "Build the ILI9341 driver"      ON)
option(JPICO_ENABLE_SSD1306   "Build the SSD1306 driver"      ON)
option(JPICO_ENABLE_XPT2046   "Build the XPT2046 touch driver" ON)
option(JPICO_ENABLE_EXAMPLES  "Build example programs"         OFF)

add_subdirectory(core)
add_subdirectory(hal)

if(JPICO_ENABLE_GRAPHICS)
    add_subdirectory(graphics)
endif()

add_subdirectory(drivers)

if(JPICO_ENABLE_NETWORK)
    add_subdirectory(network)
endif()

if(JPICO_ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()
