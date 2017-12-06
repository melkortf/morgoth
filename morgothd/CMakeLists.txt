find_package (Qt5 5.8 REQUIRED COMPONENTS Core DBus)

add_executable (morgothd main.cpp)
target_link_libraries (morgothd morgoth Qt5::Core Qt5::DBus)

include (GNUInstallDirs)
set (CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

# install morgoth daemon
install (TARGETS morgothd
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# install systemd service file
configure_file (morgoth.service.in morgoth.service)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/morgoth.service
    DESTINATION /usr/lib/systemd/system
    COMPONENT configs)

# install morgoth config file
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/morgoth.conf
    DESTINATION /etc
    COMPONENT configs)
target_sources (morgothd PRIVATE morgoth.conf)

# create empty directory for the sqlite database file
install (DIRECTORY
    DESTINATION /var/lib/morgoth
    COMPONENT runtime)

# install dbus config file so morgoth can register itself to the system bus
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/dbus.conf
    DESTINATION /etc/dbus-1/system.d
    RENAME morgoth.conf
    COMPONENT configs)
target_sources (morgothd PRIVATE dbus.conf)
