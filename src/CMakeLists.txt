find_package (Qt5 5.8 REQUIRED COMPONENTS Core)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

set (morgoth_INCLUDES
    eventhandler.h
    loglistener.h
    mapchangeevent.h
    server.h
    servercoordinator.h
    servermanager.h
    serverstartedevent.h
    serverstoppedevent.h
    tmuxsessionwrapper.h
)

set (morgoth_SOURCES
    eventhandler.cpp
    loglistener.cpp
    mapchangeevent.cpp
    server.cpp
    servercoordinator.cpp
    servermanager.cpp
    serverstartedevent.cpp
    serverstoppedevent.cpp
    tmuxsessionwrapper.cpp
)

add_executable (morgoth ${morgoth_INCLUDES} ${morgoth_SOURCES} main.cpp)
set_target_properties (morgoth PROPERTIES AUTOMOC ON)
target_link_libraries (morgoth Qt5::Core)
