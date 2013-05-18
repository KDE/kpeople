
#
# Scripts
#

# Configure the scripts
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/dbus-session-begin.sh"
                "${CMAKE_CURRENT_BINARY_DIR}/dbus-session-begin.sh"
                IMMEDIATE @ONLY
)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/dbus-session-end.sh"
                "${CMAKE_CURRENT_BINARY_DIR}/dbus-session-end.sh"
                IMMEDIATE @ONLY
)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/nepomuk-sandbox-begin.sh.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nepomuk-sandbox-begin.sh"
                IMMEDIATE @ONLY
)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/nepomuk-sandbox-end.sh.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nepomuk-sandbox-end.sh"
                IMMEDIATE @ONLY
)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/runNepomukTest.sh.in"
                "${CMAKE_CURRENT_BINARY_DIR}/runNepomukTest.sh"
                IMMEDIATE @ONLY
)

