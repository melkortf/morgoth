find_package (Qt5 5.8 REQUIRED COMPONENTS Core DBus)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

set (dbus_INCLUDES
    morgothadaptor.h
    serveradaptor.h
    servermanageradaptor.h
)

set (dbus_SOURCES
    morgothadaptor.cpp
    serveradaptor.cpp
    servermanageradaptor.cpp
)

add_library (dbus STATIC
    ${dbus_INCLUDES} ${dbus_SOURCES}
    morgoth.conf
)
target_link_libraries (dbus Qt5::Core Qt5::DBus)

install (FILES morgoth.conf
    DESTINATION /etc/dbus-1/system.d
    COMPONENT configs)
