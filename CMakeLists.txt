cmake_minimum_required(VERSION 3.16)

# Set project name
project(CCUnitTests LANGUAGES CXX)

# Do the thing to grab submodule(s).
execute_process(COMMAND git submodule update --init --recursive WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


# Set root directory of CC code
set(PROJ_CODE_ROOT ${CCUnitTests_SOURCE_DIR})
message("Unit Test Source Dir[${CCUnitTests_SOURCE_DIR}]")

# Location of CC .h files
include_directories(${PROJ_CODE_ROOT}/Headers)

# Location of CC unit test .h files
include_directories(${PROJ_CODE_ROOT}/UnitTests/Headers)

# Location of CC .cpp files
file(GLOB SOURCES "${PROJ_CODE_ROOT}/UnitTests/Source/*.cpp")

# Add executable target to be built from source.
add_executable(${PROJECT_NAME} ${SOURCES})

# Build using C++17, Release configuration.
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_TYPE Release)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Compile with extra flags.
    set(GNU_WARNING_FLAGS "-Wall -Wextra -Werror -pedantic -Wno-unknown-pragmas -pthread")
    message("Adding GNU warning flags[${GNU_WARNING_FLAGS}]")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GNU_WARNING_FLAGS}")
endif()


# Set root directory of SUTL project and code
set(SUTL_PROJ_ROOT ${PROJ_CODE_ROOT}/UnitTests/SUTL)
set(SUTL_CODE_ROOT ${SUTL_PROJ_ROOT}/SimpleUnitTestLibrary)
set(SUTL_BUILD_DIR ${SUTL_PROJ_ROOT}/build)
set(SUTL_LIB_NAME SUTL.a)

# Location of SUTL .h files
include_directories(${SUTL_CODE_ROOT}/Headers)
message("SUTL Header Path[${SUTL_CODE_ROOT}/Headers]")

# Add submodule root as a subdirectory, which should cause it to be built automagically.
add_subdirectory(${SUTL_PROJ_ROOT})

# Link in SUTL
target_link_libraries(${PROJECT_NAME} SUTL)


# Remove any default prefix from output.
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")