cmake_minimum_required(VERSION 3.13.4)

project(cflood LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
add_compile_options(-Wall -Wextra)

find_package(Curses REQUIRED)

add_executable(cflood src/main.cpp src/game.cpp src/game.h)
target_link_libraries(cflood ${CURSES_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS cflood DESTINATION ${CMAKE_INSTALL_BINDIR})
