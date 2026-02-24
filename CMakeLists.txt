# !!! Dummy file! Used only for IDEs who need this for code completion
cmake_minimum_required(VERSION 3.12..3.24)
project(mmx LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS "src/*.c")

add_executable(${PROJECT_NAME} ${ALL_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    sotn-decomp/include
)
