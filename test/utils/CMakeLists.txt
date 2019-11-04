find_package (Qt5 5.10 REQUIRED COMPONENTS Core DBus)
set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_generate_dbus_interface (gameservermock.h
    org.morgoth.test.gameservermock.xml
    OPTIONS -A
)

qt5_add_dbus_adaptor (gameservermock_DBUS_ADAPTOR
    ${CMAKE_CURRENT_BINARY_DIR}/org.morgoth.test.gameservermock.xml
    gameservermock.h
    GameServerMock
)

add_executable (testinggameserver
    testinggameserver.cpp
    gameservermock.cpp
    ${gameservermock_DBUS_ADAPTOR}
)
target_link_libraries (testinggameserver PUBLIC Qt5::Core Qt5::DBus)

add_library (testutils OBJECT
    testinggameservercontroller.cpp
)
target_link_libraries (testutils PUBLIC Qt5::Core)
