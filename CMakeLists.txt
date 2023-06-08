# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)
# プロジェクト名と使用する言語を設定
project(stfi CXX)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2")
# インクルード元のフォルダの指定
include_directories(lib)
include_directories(lib/libzmq)
link_directories(lib/libzmq/macOS)
# コンパイル時のマクロを指定
add_compile_definitions(HEADLESS)
# C++のバージョンを指定
set(CMAKE_CXX_STANDARD 20)
# 必要なファイルをコピー
file(COPY "${CMAKE_SOURCE_DIR}/App/resource" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/python_setting.jsonc" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/python" DESTINATION ${CMAKE_BINARY_DIR})
# 実行ファイルを作成
add_executable(stfi src/headless.cpp src/logic.cpp src/player.cpp src/define.cpp)
target_link_libraries(stfi PRIVATE zmq)