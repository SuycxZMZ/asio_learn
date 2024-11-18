# 设置cmake的最低版本和项目名称
cmake_minimum_required(VERSION 3.14)

project(asio_learn)

# 开启生成 cpmpile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置全局编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")

# 分别为调试和发布版本设置编译选项
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置 CMP0144 策略，使用大写环境变量名
if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

include_directories(
#  ${PROJECT_SOURCE_DIR}/src
  /usr/local/include/
  /usr/local/boost_1_86_0/include/
)

# 设置项目库文件搜索路径 -L
link_directories(
  ${PROJECT_SOURCE_DIR}/lib
  /usr/local/lib
)

# add_subdirectory(src)
add_subdirectory(example)
