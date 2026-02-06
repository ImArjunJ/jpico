add_library(jpico_xpt2046 STATIC
    src/xpt2046.cpp
)

target_include_directories(jpico_xpt2046 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jpico_xpt2046 PUBLIC
    jpico_core
    jpico_hal
)
