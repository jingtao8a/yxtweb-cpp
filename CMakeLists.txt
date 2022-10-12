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
)

add_library(yxtwebcpp SHARED ${LIB_SRC})
add_library(yxtwebcpp_static STATIC ${LIB_SRC})
set_target_properties(yxtwebcpp_static PROPERTIES OUTPUT_NAME "yxtwebcpp")

set(LIB_LIB 
    yxtwebcpp
    dl 
    pthread 
    yaml-cpp)

# add_executable(test tests/test.cpp)
# target_link_libraries(test ${LIB_LIB})
# add_dependencies(test yxtwebcpp)

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

add_executable(testiomanager tests/testiomanager.cpp)
target_link_libraries(testiomanager ${LIB_LIB})
add_dependencies(testiomanager yxtwebcpp)

add_executable(testtimer tests/testtimer.cpp)
target_link_libraries(testtimer ${LIB_LIB})
add_dependencies(testtimer yxtwebcpp)

add_executable(testhook tests/testhook.cpp)
target_link_libraries(testhook ${LIB_LIB})
add_dependencies(testhook yxtwebcpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)