cmake_minimum_required(VERSION 3.14)

project(QtFrameless LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 尝试先找Qt6，如果没有再找Qt5
find_package(Qt6 COMPONENTS Widgets)
if (NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(PROJECT_SOURCES
    main.cpp
    framelesswindow.cpp
    framelesswindow.h
    windowshadow.cpp
    windowshadow.h
    titlebar.cpp
    titlebar.h
)

add_executable(QtFrameless
    ${PROJECT_SOURCES}
)

target_link_libraries(QtFrameless PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    dwmapi
)
