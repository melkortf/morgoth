find_package (Qt5 5.8 REQUIRED COMPONENTS Core Sql)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

set (morgoth_INCLUDES
    configuration.h
    consoleinterface.h
    eventhandler.h
    loglistener.h
    mapchangeevent.h
    morgothdaemon.h
    server.h
    servercoordinator.h
    servermanager.h
    serverstartedevent.h
    serverstoppedevent.h
    stdinreader.h
    tmuxsessionwrapper.h
)

set (morgoth_SOURCES
    configuration.cpp
    consoleinterface.cpp
    eventhandler.cpp
    loglistener.cpp
    mapchangeevent.cpp
    morgothdaemon.cpp
    server.cpp
    servercoordinator.cpp
    servermanager.cpp
    serverstartedevent.cpp
    serverstoppedevent.cpp
    stdinreader.cpp
    tmuxsessionwrapper.cpp
)

add_executable (morgoth
    ${morgoth_INCLUDES} ${morgoth_SOURCES}
    main.cpp
    morgoth.conf
    morgoth.service.in
)

add_subdirectory (dbus)
target_link_libraries (morgoth dbus)

target_link_libraries (morgoth Qt5::Core Qt5::Sql)

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
    DESTINATION /var/lib/morgoth)
