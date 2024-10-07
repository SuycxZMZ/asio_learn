find_package(Boost REQUIRED COMPONENTS system)

# 公共链接库
set(COMMON_LIBS Boost::system pthread)

set(TEST_SOURCES
  boostTest.cc
  syncClient.cc
  syncServer.cc
)

# 自定义函数，用于编译每个 .cc 文件为同名的可执行文件
function(add_test_executable source_file)
  get_filename_component(target_name ${source_file} NAME_WE)
  add_executable(${target_name} ${source_file})
  target_link_libraries(${target_name} ${COMMON_LIBS})
endfunction()

# 手动添加需要编译的文件
foreach(source_file ${TEST_SOURCES})
  add_test_executable(${source_file})
endforeach()

# 如果有单独的文件或需要特殊处理的，可以手动添加，例如：
set(testall_list endpoints.cc testall.cc)
add_executable(testall ${testall_list})
target_link_libraries(testall ${COMMON_LIBS})

set(asyncServer_list asyncServer.cc asyncSession.cc)
add_executable(asyncServer ${asyncServer_list})
target_link_libraries(asyncServer ${COMMON_LIBS})