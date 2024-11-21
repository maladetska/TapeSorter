include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        tape_sorter_tests
        tape_sorter_test.cpp
)

target_link_libraries(
        tape_sorter_tests
        SimpleConfigReaderLib
        TapeLib
        GTest::gtest_main
)

target_include_directories(tape_sorter_tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(tape_sorter_tests)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/input0.in DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/config0.yaml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/output0.out DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/utests)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/input1.in DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/config1.yaml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/output1.out DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/utests)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/input2.in DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/config2.yaml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/output2.out DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/utests)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/input3.in DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/config3.yaml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/static/output3.out DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/utests)