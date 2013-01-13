project(SSHRunners)

set( QT_USE_QTXML True )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_subdirectory(krunner-ssh)

