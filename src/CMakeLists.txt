find_package (Qt5 5.8 REQUIRED COMPONENTS Core)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

set (morgoth_INCLUDES
    loglistener.h
    server.h
    servercoordinator.h
    servermanager.h
    tmuxsessionwrapper.h
)

set (morgoth_SOURCES
    loglistener.cpp
    server.cpp
    servercoordinator.cpp
    servermanager.cpp
    tmuxsessionwrapper.cpp
)

add_executable (morgoth ${morgoth_INCLUDES} ${morgoth_SOURCES} main.cpp)
set_target_properties (morgoth PROPERTIES AUTOMOC ON)
target_link_libraries (morgoth Qt5::Core)
