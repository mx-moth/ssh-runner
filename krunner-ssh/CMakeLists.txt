include_directories( {$QT_INCLUDES} ${KDE4_INCLUDES} )

set(krunner_ssh_SRCS
    ssh.cpp
)

kde4_add_plugin(krunner_ssh ${krunner_ssh_SRCS})
target_link_libraries(krunner_ssh ${KDE4_KIO_LIBS} ${KDE4_PLASMA_LIBS} ${QT_QTXML_LIBRARY} )

install(TARGETS krunner_ssh DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-ssh.desktop DESTINATION ${SERVICES_INSTALL_DIR})

install(DIRECTORY icons/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/ssh-runner)
