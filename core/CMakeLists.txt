add_library(jpico_core INTERFACE)

target_include_directories(jpico_core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(jpico_core INTERFACE cxx_std_23)
