find_package (Qt5 REQUIRED COMPONENTS Test DBus)

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory (utils)

add_executable (testserver testserver.cpp)
target_link_libraries (testserver morgoth Qt5::Test Qt5::DBus)
add_test (NAME TestServer COMMAND testserver)

add_executable (testservercoordinator testservercoordinator.cpp)
target_link_libraries (testservercoordinator morgoth Qt5::Test)
add_test (NAME TestServerCoordinator COMMAND testservercoordinator)

add_executable (testuserprocess testuserprocess.cpp)
target_link_libraries (testuserprocess morgoth Qt5::Test)
add_test (NAME TestUserProcess COMMAND testuserprocess)

add_executable (testtmuxsessionwrapper testtmuxsessionwrapper.cpp)
target_link_libraries (testtmuxsessionwrapper morgoth Qt5::Test)
add_test (NAME TestTmuxSessionWrapper COMMAND testtmuxsessionwrapper)

add_executable (teststeamid teststeamid.cpp)
target_link_libraries (teststeamid morgoth Qt5::Test)
add_test (NAME TestSteamId COMMAND teststeamid)

add_executable (testplayerinfo testplayerinfo.cpp)
target_link_libraries (testplayerinfo morgoth Qt5::Test)
add_test (NAME TestPlayerInfo COMMAND testplayerinfo)

include(CodeCoverage)
setup_target_for_coverage_lcov(
    NAME coverage
    EXECUTABLE ctest -V
    DEPENDENCIES
        testserver
        testservercoordinator
        testuserprocess
        testtmuxsessionwrapper
        teststeamid
        testplayerinfo
)


find_program (dbus_test_runner NAMES dbus-test-runner)
if (dbus_test_runner)
    add_executable (testservermanager testservermanager.cpp $<TARGET_OBJECTS:testutils>)
    target_link_libraries (testservermanager morgoth Qt5::Test Qt5::DBus)
    add_test (
        NAME TestServerManager
        COMMAND ${dbus_test_runner} -t $<TARGET_FILE:testservermanager>
    )

    add_executable (testserverstatus testserverstatus.cpp $<TARGET_OBJECTS:testutils>)
    target_link_libraries (testserverstatus morgoth Qt5::Test Qt5::DBus)
    add_test (
        NAME TestServerStatus
        COMMAND ${dbus_test_runner} -t $<TARGET_FILE:testserverstatus>
    )

    add_dependencies (coverage
        testservermanager
        testserverstatus
    )
endif ()
