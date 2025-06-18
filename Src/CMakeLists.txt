cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable not set!")
endif()

if(NOT DEFINED ENV{FREERTOS_KERNEL_RPI_PATH})
    message(FATAL_ERROR "FREERTOS_KERNEL_RPI_PATH environment variable not set!")
endif()

# Set the executable name (this will be the name of the output binary)
set(NAME pico-timer)
set(PICO_BOARD pico2)
set(PICO_PLATFORM rp2350)

# Pull in SDK (must be before project)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initialize the SDK
pico_sdk_init()

SET(FREERTOS_KERNEL_RPI_PATH "$ENV{PICO_FREERTOS}" CACHE STRING "Course Common Lib")
include("${CMAKE_CURRENT_LIST_DIR}/FreeRTOSKernelPort/freertos_config.cmake") 
include("${CMAKE_CURRENT_LIST_DIR}/FreeRTOSKernelPort/FreeRTOS_Kernel_import.cmake") 

# add_subdirectory(Src)
add_executable(${NAME}
        ./application/main.cpp
        ./application/HD44780.cpp
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${NAME} 
    pico_stdlib 
	FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
    freertos_config
    )

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

