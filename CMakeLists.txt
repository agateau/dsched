cmake_minimum_required(VERSION 3.0)

# Dependencies
find_package(Qt5 CONFIG REQUIRED Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)
