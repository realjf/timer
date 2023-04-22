cmake_minimum_required(VERSION 3.12)

set(CTIMER_VERSION 1.0)
set(PROJECT_NAME ctimer)
project(${PROJECT_NAME} VERSION ${CTIMER_VERSION} LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wno-format -O3")

# set executable output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../bin_test)

# set library output path
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../lib)

include_directories(${CMAKE_SOURCE_DIR}/../src)
include_directories(${CMAKE_SOURCE_DIR}/../deps/gtest/googletest/include)
include_directories(${CMAKE_SOURCE_DIR}/../deps/gtest/googlemock/include)

set(CTIMER_SRS ../src/timer.cpp ../src/timer_heap.cpp)
set(CTIMER_HEADERS ../src/timer.h ../src/timer_task.h ../src/timer_wheel.h ../src/timer_heap.h ../src/spinlock.h ../src/log.h)

# set library output path
# set(LIBRARY_OUTPUT_DIRECTORY lib)
# set(LIBRARY_OUTPUT_PATH lib)
add_library(${PROJECT_NAME} SHARED ${CTIMER_SRS} ${CTIMER_HEADERS})
add_library(${PROJECT_NAME}static STATIC ${CTIMER_SRS} ${CTIMER_HEADERS})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if(UNIX)
    # On unix-like platforms the library is almost always called libctimer
    set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
elseif(BUILD_SHARED_LIBS AND WIN32)
    # Creates ctimer.dll when building shared library version
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll")
endif()

add_executable(${PROJECT_NAME}_test main.cpp ${CTIMER_SRS} ${CTIMER_HEADERS})
target_link_directories(${PROJECT_NAME}_test PUBLIC ${LIBRARY_OUTPUT_PATH})
target_link_libraries(${PROJECT_NAME}_test ctimer mock gtest gtest_main mock_main)

# add_executable(spinlock_test spinlock_test.cpp ../src/spinlock.h)
