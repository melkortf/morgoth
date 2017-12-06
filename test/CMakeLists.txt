find_package (Qt5 REQUIRED COMPONENTS Test)

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable (testserver testserver.cpp)
target_link_libraries (testserver morgoth Qt5::Test)
add_test (NAME TestServer COMMAND testserver)

