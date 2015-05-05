CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(SET CMP0003 NEW)

PROJECT(geohex3)

SET(CMAKE_C_FLAGS "-g -std=gnu99 -O2 -Wall -Wno-unused-function ${CMAKE_C_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS    "-lm ${CMAKE_EXE_LINKER_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "-lm ${CMAKE_MODULE_LINKER_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "-lm ${CMAKE_SHARED_LINKER_FLAGS}")

INCLUDE_DIRECTORIES(include deps/picotest)

ADD_LIBRARY(geohex3 STATIC lib/geohex3.c)

ADD_EXECUTABLE(examples-get-zone-by-code examples/get-zone-by-code.c)
TARGET_LINK_LIBRARIES(examples-get-zone-by-code geohex3)

ADD_EXECUTABLE(test-bin test.c deps/picotest/picotest.c)
TARGET_LINK_LIBRARIES(test-bin geohex3)

ADD_CUSTOM_TARGET(test prove ./test-bin
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS geohex3 test-bin)
