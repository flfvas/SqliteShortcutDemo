cmake_minimum_required(VERSION 3.16)
project(SqliteShortcutDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# --- 使用绝对路径引用 QHotkey 源码 ---
# 请确保此路径指向您图片中显示的 addons/QHotkey 文件夹
set(QHOTKEY_DIR "F:/Qt/SqliteShortcutDemo/addons/QHotkey")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${QHOTKEY_DIR}/qhotkey.cpp
        ${QHOTKEY_DIR}/qhotkey_win.cpp
        ${QHOTKEY_DIR}/qhotkey.h
        ${QHOTKEY_DIR}/qhotkey_p.h
)

include_directories(${QHOTKEY_DIR})

add_executable(SqliteShortcutDemo ${PROJECT_SOURCES})

target_link_libraries(SqliteShortcutDemo PRIVATE
    Qt6::Widgets
    Qt6::Sql
)
