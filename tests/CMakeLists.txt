find_package(GTest REQUIRED)

enable_testing()

add_executable(tests tests.cpp)
add_test(tests tests.cpp)
target_include_directories(tests PUBLIC ${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(tests RGVM GTest::gtest GTest::gtest_main)
