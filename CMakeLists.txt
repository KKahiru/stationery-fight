# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)
# プロジェクト名と使用する言語を設定
project(headless_stfi CXX)
# インクルード元のフォルダの指定
include_directories(lib)
# コンパイル時のマクロを指定
add_compile_definitions(HEADLESS)
# C++のバージョンを指定
set(CMAKE_CXX_STANDARD 20)
# 実行ファイルを作成
add_executable(stfi src/headless.cpp src/logic.cpp src/define.cpp)
# resourceフォルダをコピー
file(COPY "App/resource" DESTINATION ${CMAKE_BINARY_DIR})