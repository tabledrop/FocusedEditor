cmake_minimum_required(VERSION 3.16)
project(focused_editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(focused_editor
    main.cpp
    editor_window.cpp
    editor_window.h
    preferences_dialog.cpp
    preferences_dialog.h
    code_highlighter.cpp
    code_highlighter.h
    indent_manager.cpp
    indent_manager.h
)

target_link_libraries(focused_editor PRIVATE Qt6::Widgets)
