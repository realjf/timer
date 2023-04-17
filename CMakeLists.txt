
cmake_minimum_required(VERSION 3.12)

set(CTIMER_VERSION 1.0)
set(PROJECT_NAME ctimer)
project(${PROJECT_NAME} VERSION ${CTIMER_VERSION} LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wno-format -O3")

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
