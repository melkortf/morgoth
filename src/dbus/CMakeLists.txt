find_package (Qt5 5.8 REQUIRED COMPONENTS Core DBus)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

set (dbus_INCLUDES
    servermanageradaptor.h
)

set (dbus_SOURCES
    servermanageradaptor.cpp
)

add_library (dbus STATIC ${dbus_INCLUDES} ${dbus_SOURCES})
target_link_libraries (dbus Qt5::Core Qt5::DBus)
