cmake_minimum_required(VERSION 3.12)

# set library output path
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../../lib)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../../../lib)

include(${CMAKE_BINARY_DIR}/../../cmake/comm_macros.cmake)

set(BUILD_SHARED_LIBS ON)

add_subdirectory(${CMAKE_BINARY_DIR}/../../deps/gtest ${CMAKE_BINARY_DIR}/build)
add_custom_target(gtestmock ALL)
add_dependencies(gtestmock gtest gmock gtest_main gmock_main)

# add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/lib/libgtest_main.so
# COMMAND echo "cope file..."
# COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/lib/libgtest_main.so ${LIBRARY_OUTPUT_DIRECTORY}
# )
if(UNIX)
    COPY_FILE(gtestmock ${CMAKE_BINARY_DIR}/lib/libgtest_main.so ${LIBRARY_OUTPUT_DIRECTORY})
    COPY_FILE(gtestmock ${CMAKE_BINARY_DIR}/lib/libgmock_main.so ${LIBRARY_OUTPUT_DIRECTORY})
    COPY_FILE(gtestmock ${CMAKE_BINARY_DIR}/lib/libgtest.so ${LIBRARY_OUTPUT_DIRECTORY})
    COPY_FILE(gtestmock ${CMAKE_BINARY_DIR}/lib/libgmock.so ${LIBRARY_OUTPUT_DIRECTORY})
    COPY_FILE(gtestmock ${CMAKE_BINARY_DIR}/lib/libgtest.so.1.13.0 ${LIBRARY_OUTPUT_DIRECTORY})
    COPY_FILE(gtestmock ${CMAKE_BINARY_DIR}/lib/libgmock.so.1.13.0 ${LIBRARY_OUTPUT_DIRECTORY})
    COPY_FILE(gtestmock ${CMAKE_BINARY_DIR}/lib/libgtest_main.so.1.13.0 ${LIBRARY_OUTPUT_DIRECTORY})
    COPY_FILE(gtestmock ${CMAKE_BINARY_DIR}/lib/libgmock_main.so.1.13.0 ${LIBRARY_OUTPUT_DIRECTORY})
endif()
