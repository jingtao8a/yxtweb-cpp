cmake_minimum_required(VERSION 3.18)
project("yxtwebcpp")
find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)
message("yaml-cpp", ${YAMLCPP})
message("pthread", ${PTHREAD})
set(LIB_SRC 
    yxtwebcpp/log.cpp 
    yxtwebcpp/util.cpp 
    yxtwebcpp/config.cpp 
    yxtwebcpp/env.cpp 
    yxtwebcpp/thread.cpp 
    yxtwebcpp/mutex.cpp
    yxtwebcpp/fiber.cpp
    yxtwebcpp/scheduler.cpp
    yxtwebcpp/iomanager.cpp
    yxtwebcpp/timer.cpp
    yxtwebcpp/hook.cpp
    yxtwebcpp/fd_manager.cpp
    yxtwebcpp/address.cpp
    yxtwebcpp/socket.cpp
    yxtwebcpp/bytearray.cpp
    yxtwebcpp/http/http.cpp
    yxtwebcpp/http/http11_parser.cpp
    yxtwebcpp/http/httpclient_parser.cpp
    yxtwebcpp/http/http_parser.cpp
    yxtwebcpp/tcpserver.cpp
)

add_library(yxtwebcpp SHARED ${LIB_SRC})
add_library(yxtwebcpp_static STATIC ${LIB_SRC})
set_target_properties(yxtwebcpp_static PROPERTIES OUTPUT_NAME "yxtwebcpp")

set(LIB_LIB 
    yxtwebcpp
    dl
    pthread 
    yaml-cpp)

add_executable(test tests/test.cpp)
target_link_libraries(test ${LIB_LIB})
add_dependencies(test yxtwebcpp)

# add_executable(test_thread tests/test_thread.cpp)
# target_link_libraries(test_thread ${LIB_LIB})
# add_dependencies(test_thread yxtwebcpp)

# add_executable(testmutex tests/testmutex.cpp)
# target_link_libraries(testmutex ${LIB_LIB})
# add_dependencies(testmutex yxtwebcpp)

# add_executable(test_config_log tests/test_config_log.cpp)
# target_link_libraries(test_config_log ${LIB_LIB})
# add_dependencies(test_config_log yxtwebcpp)

# add_executable(testutil tests/testutil.cpp)
# target_link_libraries(testutil ${LIB_LIB})
# add_dependencies(testutil yxtwebcpp)

# add_executable(testfiber tests/testfiber.cpp)
# target_link_libraries(testfiber ${LIB_LIB})
# add_dependencies(testfiber yxtwebcpp)

# add_executable(testscheduler tests/testscheduler.cpp)
# target_link_libraries(testscheduler ${LIB_LIB})
# add_dependencies(testscheduler yxtwebcpp)

# add_executable(testiomanager tests/testiomanager.cpp)
# target_link_libraries(testiomanager ${LIB_LIB})
# add_dependencies(testiomanager yxtwebcpp)

# add_executable(testtimer tests/testtimer.cpp)
# target_link_libraries(testtimer ${LIB_LIB})
# add_dependencies(testtimer yxtwebcpp)

# add_executable(testhook tests/testhook.cpp)
# target_link_libraries(testhook ${LIB_LIB})
# add_dependencies(testhook yxtwebcpp)

# add_executable(testaddress tests/testaddress.cpp)
# target_link_libraries(testaddress ${LIB_LIB})
# add_dependencies(testaddress yxtwebcpp)

# add_executable(testA tests/testA.cpp)
# target_link_libraries(testA ${LIB_LIB})
# add_dependencies(testA yxtwebcpp)

# add_executable(testB tests/testB.cpp)
# target_link_libraries(testB ${LIB_LIB})
# add_dependencies(testB yxtwebcpp)

# add_executable(testbytearray tests/testbytearray.cpp)
# target_link_libraries(testbytearray ${LIB_LIB})
# add_dependencies(testbytearray yxtwebcpp)

# add_executable(testhttp tests/testhttp.cpp)
# target_link_libraries(testhttp ${LIB_LIB})
# add_dependencies(testhttp yxtwebcpp)

# add_executable(test_http_parser tests/test_http_parser.cpp)
# target_link_libraries(test_http_parser ${LIB_LIB})
# add_dependencies(test_http_parser yxtwebcpp)

add_executable(test_tcpserver tests/test_tcpserver.cpp)
target_link_libraries(test_tcpserver ${LIB_LIB})
add_dependencies(test_tcpserver yxtwebcpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)