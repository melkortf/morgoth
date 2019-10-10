find_package (Qt5 5.10 REQUIRED COMPONENTS Core DBus Sql)
set (CMAKE_AUTOMOC ON)

set (morgoth_PUBLIC_HEADERS
    morgothdaemon.h
    morgothplugin.h
    persistor.h
    playerinfo.h
    pluginmanager.h
    server.h
    serverconfiguration.h
    servercoordinator.h
    servermanager.h
    serverstatus.h
    steamid.h
    tmuxsessionwrapper.h
    userprocess.h
)

set (morgoth_SOURCES
    morgothdaemon.cpp
    persistor.cpp
    playerinfo.cpp
    pluginmanager.cpp
    server.cpp
    serverconfiguration.cpp
    servercoordinator.cpp
    servermanager.cpp
    serverstatus.cpp
    steamid.cpp
    tmuxsessionwrapper.cpp
    userprocess.cpp
)

qt5_add_dbus_interface (morgoth_SOURCES interfaces/org.morgoth.connector.GameServer.xml gameserverinterface)

add_library (morgoth SHARED
    ${morgoth_PUBLIC_HEADERS}
    ${morgoth_PRIVATE_HEADERS}
    ${morgoth_SOURCES}
)

target_compile_options (morgoth PUBLIC $<$<CONFIG:Debug>:--coverage -fprofile-arcs -ftest-coverage>)

qt5_add_dbus_adaptor (morgoth_DBUS_ADAPTORS
    interfaces/org.morgoth.Daemon.xml morgothdaemon.h morgoth::MorgothDaemon)
qt5_add_dbus_adaptor (morgoth_DBUS_ADAPTORS
    interfaces/org.morgoth.PluginManager.xml pluginmanager.h morgoth::PluginManager)
qt5_add_dbus_adaptor (morgoth_DBUS_ADAPTORS
    interfaces/org.morgoth.Server.xml server.h morgoth::Server)
qt5_add_dbus_adaptor (morgoth_DBUS_ADAPTORS
    interfaces/org.morgoth.ServerConfiguration.xml serverconfiguration.h morgoth::ServerConfiguration)
qt5_add_dbus_adaptor (morgoth_DBUS_ADAPTORS
    interfaces/org.morgoth.ServerCoordinator.xml servercoordinator.h morgoth::ServerCoordinator)
qt5_add_dbus_adaptor (morgoth_DBUS_ADAPTORS
    interfaces/org.morgoth.ServerManager.xml servermanager.h morgoth::ServerManager)
qt5_add_dbus_adaptor (morgoth_DBUS_ADAPTORS
    interfaces/org.morgoth.ServerStatus.xml serverstatus.h morgoth::ServerStatus)
target_sources (morgoth PRIVATE ${morgoth_DBUS_ADAPTORS})

configure_file (config.h.in config.h)
target_sources (morgoth PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/config.h)

target_include_directories (morgoth
    PUBLIC
        $<INSTALL_INTERFACE:include/morgoth> # <prefix>/include/morgoth
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries (morgoth
    PUBLIC Qt5::Core $<$<CONFIG:Debug>:gcov>
    PRIVATE Qt5::DBus Qt5::Sql
)

set (morgoth_GENERATED_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/morgoth_export.h
)
target_sources (morgoth PRIVATE ${morgoth_GENERATED_HEADERS})

set_target_properties (morgoth PROPERTIES
    VERSION ${morgoth_VERSION}
    SOVERSION 1
    CXX_VISIBILITY_PRESET hidden
    PUBLIC_HEADER "${morgoth_PUBLIC_HEADERS};${morgoth_GENERATED_HEADERS}"
)

include (GenerateExportHeader)
generate_export_header (morgoth)

include (GNUInstallDirs)
install (TARGETS morgoth
    EXPORT morgoth-config
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/morgoth
)

install (EXPORT morgoth-config
    NAMESPACE
        morgoth::
    DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/morgoth/cmake
)

install (DIRECTORY interfaces
    DESTINATION ${CMAKE_INSTALL_DATADIR}/morgoth/dbus-1)

add_subdirectory (morgothd)
add_subdirectory (morgothctl)
