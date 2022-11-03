cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_FLAGS "-std=c++11 -g")
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
    yxtwebcpp/streams/stream.cpp
    yxtwebcpp/streams/socket_stream.cpp
    yxtwebcpp/http/httpstream.cpp
    yxtwebcpp/http/httpserver.cpp
    yxtwebcpp/http/servlet.cpp
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

add_executable(test_httpserver tests/test_httpserver.cpp)
target_link_libraries(test_httpserver ${LIB_LIB})
add_dependencies(test_httpserver yxtwebcpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


set(head_files  
        yxtwebcpp/address.hpp
        yxtwebcpp/env.hpp
        yxtwebcpp/nocopyable.hpp
        yxtwebcpp/singleton.hpp
        yxtwebcpp/timer.hpp
        yxtwebcpp/log.hpp
        yxtwebcpp/config.hpp
        yxtwebcpp/thread.hpp
        yxtwebcpp/mutex.hpp
        yxtwebcpp/util.hpp
        yxtwebcpp/macro.hpp
        yxtwebcpp/fiber.hpp
        yxtwebcpp/scheduler.hpp
        yxtwebcpp/iomanager.hpp
        yxtwebcpp/hook.hpp
        yxtwebcpp/fd_manager.hpp
        yxtwebcpp/address.hpp
        yxtwebcpp/endian.hpp
        yxtwebcpp/socket.hpp
        yxtwebcpp/bytearray.hpp
        yxtwebcpp/tcpserver.hpp        
        yxtwebcpp/yxtwebcpp.hpp
        )

set(head_http_files
        yxtwebcpp/http/http.hpp
        yxtwebcpp/http/http11_parser.hpp
        yxtwebcpp/http/httpclient_parser.hpp
        yxtwebcpp/http/http_parser.hpp
        yxtwebcpp/http/httpserver.hpp)

set(head_streams_files
        yxtwebcpp/streams/stream.hpp
        yxtwebcpp/streams/socket_stream.hpp)

install(FILES ${head_files} DESTINATION  include/yxtwebcpp)
install(FILES ${head_http_files} DESTINATION include/yxtwebcpp/http)
install(FILES ${head_streams_files} DESTINATION include/yxtwebcpp/streams)

install(TARGETS yxtwebcpp yxtwebcpp_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)