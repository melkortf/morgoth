find_package (Qt5 5.8 REQUIRED COMPONENTS Core DBus)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

set (morgothctl_SOURCES main.cpp)

set (server_IFACE_XML ${CMAKE_CURRENT_SOURCE_DIR}/../server.xml)
set_source_files_properties (${server_IFACE_XML} PROPERTIES INCLUDE "serverlauncharguments.h")
qt5_add_dbus_interface(morgothctl_SOURCES ${server_IFACE_XML} serverinterface)

set (servercoordinator_IFACE_XML ${CMAKE_CURRENT_SOURCE_DIR}/../servercoordinator.xml)
set_source_files_properties (${servercoordinator_IFACE_XML} PROPERTIES INCLUDE "servercoordinator.h")
qt5_add_dbus_interface(morgothctl_SOURCES ${servercoordinator_IFACE_XML} servercoordinatorinterface)

qt5_add_dbus_interface(morgothctl_SOURCES ../servermanager.xml servermanagerinterface)

add_executable (morgothctl ${morgothctl_SOURCES})
target_link_libraries (morgothctl Qt5::Core Qt5::DBus morgoth)
