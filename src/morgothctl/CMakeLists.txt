find_package (Qt5 5.8 REQUIRED COMPONENTS Core DBus)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

set (morgothctl_SOURCES main.cpp)

set (morgoth_IFACES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../interfaces)

set (servercoordinator_IFACE_XML ${morgoth_IFACES_DIR}/org.morgoth.ServerCoordinator.xml)
set_source_files_properties (${servercoordinator_IFACE_XML} PROPERTIES INCLUDE "servercoordinator.h")
qt5_add_dbus_interface(morgothctl_SOURCES ${servercoordinator_IFACE_XML} servercoordinatorinterface)

qt5_add_dbus_interface(morgothctl_SOURCES ${morgoth_IFACES_DIR}/org.morgoth.Server.xml serverinterface)
qt5_add_dbus_interface(morgothctl_SOURCES ${morgoth_IFACES_DIR}/org.morgoth.ServerManager.xml servermanagerinterface)

add_executable (morgothctl ${morgothctl_SOURCES})
target_link_libraries (morgothctl Qt5::Core Qt5::DBus morgoth)

include (GNUInstallDirs)
install (TARGETS morgothctl
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT binaries)
