# 设置cmake的最低版本和项目名称
cmake_minimum_required(VERSION 3.14)

project(asio_learn)

# 开启生成 cpmpile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# 设置全局编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -std=c++20")

# 分别为调试和发布版本设置编译选项
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
include_directories(
#  ${PROJECT_SOURCE_DIR}/src
  /usr/local/include/
)

# 设置项目库文件搜索路径 -L
link_directories(
  ${PROJECT_SOURCE_DIR}/lib
  /usr/local/lib
)

# add_subdirectory(src)
add_subdirectory(example)
