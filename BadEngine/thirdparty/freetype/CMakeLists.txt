# Set the minimum CMake version
cmake_minimum_required(VERSION 3.21)

# Project name
project(FreeType VERSION 2.13.2)

# Add the source files
set(FREETYPE_SOURCES
    #src/base/ftadvanc.c
    #src/base/ftapi.c
    # Add more source files here...
)

# Add the include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # Add more include directories if needed...
)

# Create the FreeType library
#add_library(freetype STATIC ${FREETYPE_SOURCES})

# Set target properties if needed
# For example:
# set_target_properties(freetype PROPERTIES CXX_STANDARD 11)

# Optionally, you might want to install the library
# install(TARGETS freetype DESTINATION lib)
