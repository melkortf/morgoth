find_package (Qt5 REQUIRED COMPONENTS Test)

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable (testserver testserver.cpp)
target_link_libraries (testserver morgoth Qt5::Test)
add_test (NAME TestServer COMMAND testserver)

add_executable (testservermanager testservermanager.cpp)
target_link_libraries (testservermanager morgoth Qt5::Test)
add_test (NAME TestServerManager COMMAND testservermanager)

add_executable (testuserprocess testuserprocess.cpp)
target_link_libraries (testuserprocess morgoth Qt5::Test)
add_test (NAME TestUserProcess COMMAND testuserprocess)

add_executable (testtmuxsessionwrapper testtmuxsessionwrapper.cpp)
target_link_libraries (testtmuxsessionwrapper morgoth Qt5::Test)
add_test (NAME TestTmuxSessionWrapper COMMAND testtmuxsessionwrapper)

add_executable (teststeamid teststeamid.cpp)
target_link_libraries (teststeamid morgoth Qt5::Test)
add_test (NAME TestSteamId COMMAND teststeamid)
