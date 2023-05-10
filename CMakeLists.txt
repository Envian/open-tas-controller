cmake_minimum_required(VERSION 3.12)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(open-tas-controller C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNDEBUG")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-ignored-qualifiers")

pico_sdk_init()

file(GLOB SRC_FILES CONFIGURE_DEPENDS "./src/*.cpp" "./src/**/*.cpp")
file(GLOB PIO_FILES CONFIGURE_DEPENDS "./pio/*.pio")

include_directories(./include ./pio)
add_executable(open-tas-controller ${SRC_FILES})
pico_generate_pio_header(open-tas-controller ${PIO_FILES})

target_link_libraries(open-tas-controller
    pico_stdlib
    pico_multicore
    hardware_pio
    # tinyusb_device
    # tinyusb_board
)

pico_add_extra_outputs(open-tas-controller)
pico_enable_stdio_usb(open-tas-controller 1)
pico_enable_stdio_uart(open-tas-controller 0)

add_custom_command(TARGET open-tas-controller POST_BUILD
    COMMAND "picotool" "load" "-fx" "open-tas-controller.uf2"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/open-tas-controller.uf2"
)

add_custom_command(TARGET open-tas-controller POST_BUILD
    COMMAND "picotool" "info" "-a" "open-tas-controller.uf2" "|" "grep" "binary"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/open-tas-controller.uf2"
)


# Currently does not output the message asking the user to put pico into USB mode.
# Until I can print a message here sucessfully, it looks like a hang.


# add_custom_command(TARGET open-tas-controller POST_BUILD
#     COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/upload.sh" ${CMAKE_CURRENT_BINARY_DIR}
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/open-tas-controller.uf2"
# )
