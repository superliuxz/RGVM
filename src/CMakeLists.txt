find_package(Boost REQUIRED COMPONENTS system)

add_library(RGVM SHARED parser.cpp instructions.cpp RGVM.cpp)
target_include_directories(RGVM PUBLIC ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
