find_package (Qt5 REQUIRED COMPONENTS Core DBus)

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

set (morgothctl_SOURCES
    main.cpp
    morgothctlplugin.h

    addcommand.h
    configcommand.h
    consolecommand.h
    listcommand.h
    removecommand.h
    restartcommand.h
    startcommand.h
    statuscommand.h
    stopcommand.h

    addcommand.cpp
    configcommand.cpp
    consolecommand.cpp
    listcommand.cpp
    removecommand.cpp
    restartcommand.cpp
    startcommand.cpp
    statuscommand.cpp
    stopcommand.cpp
)
set (morgoth_IFACES_DIR ${PROJECT_SOURCE_DIR}/src/interfaces)

set (servercoordinator_IFACE_XML ${morgoth_IFACES_DIR}/org.morgoth.ServerCoordinator.xml)
set_source_files_properties (${servercoordinator_IFACE_XML} PROPERTIES INCLUDE "servercoordinator.h")
qt5_add_dbus_interface (morgothctl_SOURCES ${servercoordinator_IFACE_XML} servercoordinatorinterface)

qt5_add_dbus_interface (morgothctl_SOURCES ${morgoth_IFACES_DIR}/org.morgoth.Server.xml serverinterface)
qt5_add_dbus_interface (morgothctl_SOURCES ${morgoth_IFACES_DIR}/org.morgoth.ServerConfiguration.xml serverconfigurationinterface)
qt5_add_dbus_interface (morgothctl_SOURCES ${morgoth_IFACES_DIR}/org.morgoth.ServerManager.xml servermanagerinterface)

set (serverstatus_IFACE_XML ${morgoth_IFACES_DIR}/org.morgoth.ServerStatus.xml)
set_source_files_properties (${serverstatus_IFACE_XML} PROPERTIES INCLUDE "playerinfo.h")
qt5_add_dbus_interface (morgothctl_SOURCES ${serverstatus_IFACE_XML} serverstatusinterface)

add_executable (morgothctl ${morgothctl_SOURCES})
target_link_libraries (morgothctl Qt5::Core Qt5::DBus morgoth)

include (GNUInstallDirs)

set_target_properties (morgothctl PROPERTIES
    INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
)

install (TARGETS morgothctl
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT binaries
)
