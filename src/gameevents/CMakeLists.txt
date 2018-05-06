set (CMAKE_AUTOMOC ON)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

set (gameevents_SOURCES
    mapchange.cpp
    playerconnected.cpp
    playerdropped.cpp
    serverstarted.cpp
    serverstopped.cpp
    statushostname.cpp
    statusipaddress.cpp
    statusmap.cpp
    statusplayernumbers.cpp
)

set (gameevents_HEADERS
    mapchange.h
    playerconnected.h
    playerdropped.h
    serverstarted.h
    serverstopped.h
    statushostname.h
    statusipaddress.h
    statusmap.h
    statusplayernumbers.h
)

add_library (gameevents-obj OBJECT
    ${gameevents_HEADERS}
    ${gameevents_SOURCES}
)

# https://gitlab.kitware.com/cmake/cmake/issues/14778
add_library (gameevents INTERFACE)
target_link_libraries (gameevents
    INTERFACE
        Qt5::Core
)
target_sources (gameevents
    INTERFACE
        $<TARGET_OBJECTS:gameevents-obj>
)

target_include_directories(gameevents-obj
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/..
        $<TARGET_PROPERTY:gameevents,INTERFACE_INCLUDE_DIRECTORIES>
)
