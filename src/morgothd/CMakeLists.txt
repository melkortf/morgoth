include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

add_executable (morgothd main.cpp)
target_link_libraries (morgothd morgoth)

configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/morgoth.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/morgoth.service
)

include (GNUInstallDirs)

# install morgoth daemon
install (TARGETS morgothd
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# install systemd service file
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
