#
# main CMAKE file
#
cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0074 NEW)

project("HW Verification App HOST (Version 0.0)")

#############################################################################################
# Compiler FLAGS
#############################################################################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#######################################
### Qt
#######################################
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#############################################################################################
# Executable: sim
#############################################################################################
set(module checker)

# source files for executable
set(Sources
        main.cpp
        ../../common/vpro_host_aux.cpp
        ../../../vpro/verifyHW/sources/test_framework.cpp
#        ../../../vpro/cnn_yolo_lite_hw/yolo_configuration.cpp
        )
file(GLOB_RECURSE Sources2
        "sources/*.cpp"
        )

file(GLOB_RECURSE Headers
        "*.h"
        )

set(PlainIncludeDirs
        includes/
        ../../../../../SYS/tools/libcomm/include/
        ../../../vpro/verifyHW/includes/
        ../../../isa_intrinsic_lib/
#        ../../../vpro/cnn_yolo_lite_hw/
#        ../../../vpro/cnn_yolo_lite_hw/includes/
#        ../../../vpro/cnn_converter/vpro_lib_hw/includes/
        )

add_executable(${module} ${Sources} ${Sources2} ${Headers})
target_include_directories(${module} PUBLIC ${PlainIncludeDirs})

#######################################
# Link against Uemu Comm Lib
#######################################
find_library(UEMU_LIB NAMES comm
        HINTS ../../../../../SYS/tools/libcomm)

if(NOT ${UEMU_LIB})
    set(UEMU_PATH ${PROJECT_SOURCE_DIR}/../../../../../SYS/tools/libcomm/libcomm.a)
    add_library(UEMU_LIB STATIC IMPORTED)
    set_property(TARGET UEMU_LIB PROPERTY IMPORTED_LOCATION ${UEMU_PATH})
endif()
target_link_libraries(${module} ${UEMU_LIB})

# Set UEMU Env Variables
set(CONN_MODE "SIM_IO")
target_compile_definitions(${module} PUBLIC ${CONN_MODE}=1 X86=1) # Simulation to enable c++ simulation reference functions...
#target_compile_features(${module} PUBLIC cxx_std_17)

#######################################
### Qt
#######################################
target_include_directories(${module} PUBLIC ${Qt5Core_INCLUDE_DIRS})
target_include_directories(${module} PUBLIC ${Qt5Widgets_INCLUDE_DIRS})
target_compile_definitions(${module} PUBLIC ${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
target_link_libraries(${module}  Qt5::Core Qt5::Widgets)

#######################################
### include OpenCV                  ###
#######################################
#find_package(OpenCV REQUIRED)
#include_directories(${OpenCV_INCLUDE_DIRS})
#target_link_libraries(${module} ${OpenCV_LIBS})

#######################################
### Link agains PNGLIB              ###
#######################################
#target_link_libraries(${module} ${PNG_LIBRARY})

#####################################
### Link agains BOOST             ###
#####################################
#set(Boost_DEBUG 1)
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
FIND_PACKAGE(Boost)
#find_package( Boost COMPONENTS thread REQUIRED )
message(STATUS "Boost version: ${Boost_VERSION}")
include_directories(${Boost_INCLUDE_DIR})
ADD_DEFINITIONS( "-DHAS_BOOST" )
#include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${module} ${Boost_LIBRARIES})






set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
add_executable(dump_mm dump_mm.cpp ../../common/vpro_host_aux.cpp ../../../vpro/verifyHW/sources/test_framework.cpp ${Headers})
target_include_directories(dump_mm PUBLIC ${PlainIncludeDirs})
target_link_libraries(dump_mm ${UEMU_LIB})
target_include_directories(dump_mm PUBLIC ${Qt5Core_INCLUDE_DIRS})
target_include_directories(dump_mm PUBLIC ${Qt5Widgets_INCLUDE_DIRS})
target_compile_definitions(dump_mm PUBLIC ${Qt5Widgets_DEFINITIONS})
target_link_libraries(dump_mm  Qt5::Core) # Qt5::Widgets)
target_compile_definitions(dump_mm PUBLIC ${CONN_MODE}=1 X86=1) # Simulation to enable c++ simulation reference functions...