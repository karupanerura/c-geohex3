CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(SET CMP0003 NEW)

PROJECT(geohex)

SET(CMAKE_C_FLAGS "-O2 -g -Wall -Wno-unused-function -std=gnu99 ${CMAKE_C_FLAGS}")

INCLUDE_DIRECTORIES(include deps/picotest)

ADD_LIBRARY(geohex STATIC lib/geohex.c)

ADD_EXECUTABLE(test-bin test.c deps/picotest/picotest.c)
TARGET_LINK_LIBRARIES(test-bin geohex)

ADD_CUSTOM_TARGET(test prove ./test-bin
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS geohex test-bin)
