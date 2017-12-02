find_package (Qt5 5.8 REQUIRED COMPONENTS Core DBus Sql)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

set (morgoth_INCLUDES
    eventhandler.h
    loglistener.h
    mapchangeevent.h
    morgothdaemon.h
    persistor.h
    server.h
    servercoordinator.h
    serverlauncharguments.h
    servermanager.h
    serverstartedevent.h
    serverstoppedevent.h
    tmuxsessionwrapper.h
    updatenotificationevent.h
)

set (morgoth_SOURCES
    eventhandler.cpp
    loglistener.cpp
    mapchangeevent.cpp
    morgothdaemon.cpp
    persistor.cpp
    server.cpp
    servercoordinator.cpp
    serverlauncharguments.cpp
    servermanager.cpp
    serverstartedevent.cpp
    serverstoppedevent.cpp
    tmuxsessionwrapper.cpp
    updatenotificationevent.cpp
)

qt5_add_dbus_adaptor(morgoth_SOURCES
    interfaces/org.morgoth.Server.xml server.h morgoth::Server)
qt5_add_dbus_adaptor(morgoth_SOURCES
    interfaces/org.morgoth.ServerCoordinator.xml servercoordinator.h morgoth::ServerCoordinator)
qt5_add_dbus_adaptor(morgoth_SOURCES
    interfaces/org.morgoth.ServerManager.xml servermanager.h morgoth::ServerManager)

add_library (morgoth SHARED
    ${morgoth_INCLUDES}
    ${morgoth_SOURCES}
)
target_link_libraries (morgoth Qt5::Core Qt5::DBus Qt5::Sql)
set_target_properties (morgoth PROPERTIES
    VERSION ${morgoth_VERSION}
    SOVERSION 1
    CXX_VISIBILITY_PRESET hidden
)

include (GenerateExportHeader)
generate_export_header (morgoth)

include (GNUInstallDirs)
install (TARGETS morgoth
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set (CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
add_subdirectory (morgothd)
add_subdirectory (morgothctl)
