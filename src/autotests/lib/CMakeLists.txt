#
# nepomukserverrc config file
#
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/nepomukserverrc.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nepomukserverrc"
                IMMEDIATE @ONLY
)

#
# CMake Macros
#

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/NepomukTestLibMacros.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/NepomukTestLibMacros.cmake"
                IMMEDIATE @ONLY
)

#
# Source code
#

set (nepomuk_testlib_SRCS
    datagenerator.cpp
    testbase.cpp
)

qt4_add_dbus_interface(nepomuk_testlib_SRCS ${KDE4_DBUS_INTERFACES_DIR}/org.kde.nepomuk.ServiceManager.xml nepomukservicemanagerinterface)

# nepomuktestlib is static, no need for exports
add_definitions(-DNEPOMUKTEST_EXPORT=)

kde4_add_library (nepomuktestlib
                  STATIC
                  ${nepomuk_testlib_SRCS}
)

add_dependencies(nepomuktestlib nepomukcore)

target_link_libraries (nepomuktestlib
                       ${QT_QTCORE_LIBRARY}
                       ${QT_QTDBUS_LIBRARY}
                       ${QT_QTTEST_LIBRARY}
                       ${NEPOMUK_CORE_LIBRARY}
)

set_target_properties (nepomuktestlib PROPERTIES
                       VERSION ${GENERIC_LIB_VERSION}
                       SOVERSION ${GENERIC_LIB_SOVERSION}
)

set( nepomuk_testlib_HEADERS
     testbase.h
     nepomuktest_export.h
)

add_subdirectory(tools)
