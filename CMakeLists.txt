cmake_minimum_required(VERSION 3.16)

project(focused_editor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(focused_editor
    main.cpp
    editor_window.cpp
    editor_window.h
    code_highlighter.cpp
    code_highlighter.h
    preferences_dialog.cpp
    preferences_dialog.h
    indent_manager.cpp
    indent_manager.h
    line_number_area.cpp
    line_number_area.h
    custom_editor.cpp
    custom_editor.h
)

target_link_libraries(focused_editor PRIVATE Qt6::Widgets)

set_target_properties(focused_editor PROPERTIES
    WIN32_EXECUTABLE ON
)
