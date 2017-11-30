configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/morgoth.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/morgoth.service
)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

add_executable (morgothd main.cpp)
target_link_libraries (morgothd morgoth)

include (GNUInstallDirs)
install (TARGETS morgothd
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/morgoth.service
    DESTINATION lib/systemd/system
    COMPONENT configs)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/morgoth.conf
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}
    COMPONENT configs)
install (DIRECTORY
    DESTINATION /var/lib/morgoth
    COMPONENT configs)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/dbus.conf
    DESTINATION /etc/dbus-1/system.d
    RENAME morgoth.conf
    COMPONENT configs)
