cmake_minimum_required(VERSION 3.27)
project(calculator_c C)

set(CMAKE_C_STANDARD 11)

# GTK+3をプロジェクトに追加
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# GTK+3のヘッダーファイルのパスをインクルード
include_directories(${GTK3_INCLUDE_DIRS})

# GTK+3のライブラリをリンク
link_directories(${GTK3_LIBRARY_DIRS})

# GTK+3のコンパイルフラグを追加
add_definitions(${GTK3_CFLAGS_OTHER})


add_executable(calculator_c src/main.c
        src/expression_parser.c)

# 実行可能ファイルにGTK+3ライブラリをリンク
target_link_libraries(calculator_c ${GTK3_LIBRARIES})

