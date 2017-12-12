find_package (Qt5 5.8 REQUIRED COMPONENTS Core)

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

add_library (updatenotifier MODULE
    updatenotificationevent.h
    updatenotifier.h

    updatenotificationevent.cpp
    updatenotifier.cpp

    updatenotifier.json
)

target_compile_definitions (updatenotifier
    PRIVATE -DQT_PLUGIN -DQT_SHARED
)

target_link_libraries (updatenotifier morgoth)

include (GNUInstallDirs)

set_target_properties (updatenotifier PROPERTIES
    INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
)

install (TARGETS updatenotifier
    LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/morgoth/plugins
    COMPONENT plugins
)
