cmake_minimum_required(VERSION 3.12)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build) 
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(open-tas-controller C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

file(GLOB SRC_FILES CONFIGURE_DEPENDS "./src/*.cpp")
file(GLOB PIO_FILES CONFIGURE_DEPENDS "./pio/*.pio")

include_directories(./include ./pio)
add_executable(open-tas-controller ${SRC_FILES})
pico_generate_pio_header(open-tas-controller ${PIO_FILES})

target_link_libraries(open-tas-controller 
    pico_stdlib
    pico_multicore
    hardware_pio
)

pico_add_extra_outputs(open-tas-controller)
pico_enable_stdio_usb(open-tas-controller 1)
pico_enable_stdio_uart(open-tas-controller 0)