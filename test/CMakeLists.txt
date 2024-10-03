set(boostTest_list boostTest.cc)
add_executable(boostTest ${boostTest_list})
target_link_libraries(boostTest pthread)

set(testall_list testall.cc endpoints.cc)
add_executable(testall ${testall_list})
target_link_libraries(testall pthread)