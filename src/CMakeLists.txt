find_package (Qt5 5.8 REQUIRED COMPONENTS Core DBus Sql)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

set (morgoth_INCLUDES
    configuration.h
    eventhandler.h
    loglistener.h
    mapchangeevent.h
    morgothdaemon.h
    server.h
    servercoordinator.h
    serverlauncharguments.h
    servermanager.h
    serverstartedevent.h
    serverstoppedevent.h
    tmuxsessionwrapper.h
)

set (morgoth_SOURCES
    configuration.cpp
    eventhandler.cpp
    loglistener.cpp
    mapchangeevent.cpp
    morgothdaemon.cpp
    server.cpp
    servercoordinator.cpp
    serverlauncharguments.cpp
    servermanager.cpp
    serverstartedevent.cpp
    serverstoppedevent.cpp
    tmuxsessionwrapper.cpp
)

set (morgoth_DBUSIFACES
    server.xml
    servercoordinator.xml
    servermanager.xml
)

qt5_add_dbus_adaptor(morgoth_SOURCES
    server.xml server.h morgoth::Server)
qt5_add_dbus_adaptor(morgoth_SOURCES
    servercoordinator.xml servercoordinator.h morgoth::ServerCoordinator)
qt5_add_dbus_adaptor(morgoth_SOURCES
    servermanager.xml servermanager.h morgoth::ServerManager)

add_executable (morgoth
    ${morgoth_INCLUDES} ${morgoth_SOURCES} ${morgoth_DBUSIFACES}
    config.h.in
    dbus.conf
    main.cpp
    morgoth.conf
    morgoth.service.in
)

target_link_libraries (morgoth Qt5::Core Qt5::DBus Qt5::Sql)

install (TARGETS morgoth
    DESTINATION bin
    COMPONENT binaries)

configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/morgoth.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/morgoth.service
)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/morgoth.service
    DESTINATION lib/systemd/system
    COMPONENT configs)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/morgoth.conf
    DESTINATION /etc
    COMPONENT configs)
install (DIRECTORY
    DESTINATION /var/lib/morgoth
    COMPONENT configs)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/dbus.conf
    DESTINATION /etc/dbus-1/system.d
    RENAME morgoth.conf
    COMPONENT configs)
