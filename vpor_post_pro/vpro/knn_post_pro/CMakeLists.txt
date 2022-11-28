#
# main CMAKE file
# includes libs (sim, vpro_cnn, ...)
# defines executable (sim)
#
#

cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0074 NEW)

if(NOT DEFINED PROJECT)
    get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    string(REPLACE " " "_" ProjectId ${ProjectId})
    set(PROJECT ${ProjectId})
endif(NOT DEFINED PROJECT)
project(${PROJECT})


# includes VPRO_SIMULTOR_LIB library
set(VPRO_SIMULTOR_LIB_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../isa_intrinsic_lib")
add_subdirectory(${VPRO_SIMULTOR_LIB_dir} ${CMAKE_CURRENT_BINARY_DIR}/VPRO_SIMULATOR_LIB)


#############################################################################################
# Compiler FLAGS
#############################################################################################
macro(use_cxx11)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")
        endif ()
    else ()
        set(CMAKE_CXX_STANDARD 17)
    endif ()
endmacro(use_cxx11)
use_cxx11()
set(GFLAG -std=c++17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


#############################################################################################
# Executable: sim
#############################################################################################
set(module sim)

# source files for executable
file(GLOB_RECURSE Sources
    "sources/*.cpp"
)

# source files for executable
file(GLOB_RECURSE Headers
    "includes/*.h"
)

set(PlainIncludeDirs
	includes/
	../../isa_intrinsic_lib/
)

list(APPEND Sources "../common/vpro_globals.cpp")
add_executable(${module} ${Sources} main.cpp ${Headers})

# set HW config via defines
if(NOT DEFINED SCRIPTED)
    set(SCRIPTED 1)
endif(NOT DEFINED SCRIPTED)
if(NOT DEFINED UNITS)
    set(UNITS 2)
endif(NOT DEFINED UNITS)
if(NOT DEFINED CLUSTERS)
    set(CLUSTERS 2)
endif(NOT DEFINED CLUSTERS)

message(STATUS "using CLUSTERS=${CLUSTERS}")
message(STATUS "using UNITS=${UNITS}")
message(STATUS "using COMMENT=${PROJECT}")
message(STATUS "using SCRIPTED=${SCRIPTED}")
target_compile_definitions(${module} PUBLIC SCRIPTED=${SCRIPTED} NUM_VU_PER_CLUSTER=${UNITS} NUM_CLUSTERS=${CLUSTERS} STAT_COMMENT=\"${PROJECT}\" SIMULATION=1)

# include dirs for libs
target_include_directories(${module} PUBLIC ${PlainIncludeDirs})
target_link_libraries(${module} VPRO_SIMULTOR_LIB)
 
# these are all global variables -> assign to addres above main_memory max address
target_link_options(${module} PUBLIC -Wl,--no-relax,--section-start=.rodata=0x0000000040000000)

# linker options:
#target_link_options(${module} PUBLIC -Wl,--section-start=glob=0x0000000060000000)   # these are the datas [const global arrays] defined in region glob -> assign to addres above main_memory max address
#target_link_options(${module} PUBLIC -Wl,--no-check-sections)
#target_link_options(${module} PUBLIC -Wl,--print-map)
#target_link_options(${module} PUBLIC -Wl,--verbose)


#######################################
### include VPRO CNN LIB            ###
#######################################
# includes VPRO_CNN_LIB Library
#set(VPRO_CNN_LIB_dir "${CMAKE_CURRENT_SOURCE_DIR}/../cnn_converter/vpro_lib")
#add_subdirectory(${VPRO_CNN_LIB_dir} ${CMAKE_CURRENT_BINARY_DIR}/VPRO_CNN_LIB)
#target_include_directories(${module} PUBLIC ../cnn_converter/vpro_lib/includes/)
## Depends on Simulator
#target_include_directories(VPRO_CNN_LIB PUBLIC ../../../isa_intrinsic_lib/)
#target_link_libraries(VPRO_CNN_LIB VPRO_SIMULTOR_LIB)
#target_compile_definitions(VPRO_SIMULTOR_LIB PUBLIC NUM_VU_PER_CLUSTER=${UNITS} NUM_CLUSTERS=${CLUSTERS} STAT_COMMENT=\"${PROJECT}\")
#target_link_libraries(${module} VPRO_CNN_LIB)


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


#######################################
### Link agains Darknet             ###
#######################################
#find_library(DARKNET_LIBRARY NAMES darknet
#             HINTS ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR})
#
#target_link_libraries(${module} ${DARKNET_LIBRARY})


#######################################
### Link agains XTensor             ###
#######################################
#find_package(xtl REQUIRED PATHS ${CMAKE_SOURCE_DIR}/lib/xtl/install/)
#find_package(xtensor REQUIRED PATHS ${CMAKE_SOURCE_DIR}/lib/xtensor/install/)
#target_include_directories(${module} PUBLIC ${xtensor_INCLUDE_DIRS})
#target_link_libraries(${module}  xtensor)


#####################################
### Link agains BOOST             ###
#####################################
#set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
#set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
#set(Boost_USE_MULTITHREADED      ON)
#find_package( Boost 1.65.1 COMPONENTS thread REQUIRED )
#message(STATUS "Boost version: ${Boost_VERSION}")
#include_directories(${Boost_INCLUDE_DIR})
#include_directories(${Boost_INCLUDE_DIRS})
#target_link_libraries(${module} ${Boost_LIBRARIES})
