add_executable(example main.cpp)
target_link_libraries(example RGVM)
target_include_directories(example PUBLIC ${PROJECT_SOURCE_DIR}/src)