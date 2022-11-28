# LIBRARY
#
# VPRO Simulator
#
# offers all vpro functions and simulates them

# usage:
# in main cmake file (for the app using this lib):
#
# set(VPRO_SIMULTOR_LIB_dir "../../isa_intrinsic_lib")
# add_subdirectory(${VPRO_SIMULTOR_LIB_dir})


cmake_minimum_required(VERSION 3.14)
set(LIB_NAME "VPRO_SIMULTOR_LIB")

# Source Files for this lib
file(GLOB_RECURSE Sources 
	"*.cpp" 
	"*.hpp"
)

# remove main from GUI from simulator sources
list(REMOVE_ITEM Sources ${CMAKE_CURRENT_SOURCE_DIR}/simulator/windows/Commands/main.cpp)

# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.
file(GLOB_RECURSE Headers 
 	"*.h" 
 	"*.hh"
)

set(LibIncludeDirs 
	./
	simulator/windows/Commands/
)


#message("Compile of sim lib: CMAKE_CURRENT_SOURCE_DIR:")
#message(${CMAKE_CURRENT_SOURCE_DIR})
#message("Compile of sim lib: PROJECT_SOURCE_DIR:")
#message(${PROJECT_SOURCE_DIR})


find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/simulator/windows/Commands)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Make an automatic library - will be static or dynamic based on user setting
add_library(${LIB_NAME} ${Sources} ${Headers})

## This depends on QT
target_include_directories(${LIB_NAME} PUBLIC ${Qt5Core_INCLUDE_DIRS})
target_include_directories(${LIB_NAME} PUBLIC ${Qt5Widgets_INCLUDE_DIRS})
target_compile_definitions(${LIB_NAME} PUBLIC ${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
target_link_libraries(${LIB_NAME}  Qt5::Core Qt5::Widgets)
#set(CMAKE_AUTOGEN_VERBOSE ON)

# We need this directory, and users of our library will need it too
target_include_directories(${LIB_NAME} PUBLIC ${LibIncludeDirs})

# All users of this library will need at least C++17
target_compile_features(${LIB_NAME} PUBLIC cxx_std_17)

# defines for core
target_compile_definitions(${LIB_NAME} PUBLIC IS_SIMULATION=1)

# IDEs should put the headers in a nice place
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Header Files SIM" FILES ${Headers})



#add_library(SIMLIB SHARED ${SimSources})

