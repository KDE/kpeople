add_subdirectory(plugins)

include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set (kpeople_widgets_SRCS
    abstractpersondetailswidget.cpp
    persondetailsview.cpp
    mergedialog.cpp
    mergedelegate.cpp
)

kde4_add_library (kpeoplewidgets SHARED ${kpeople_widgets_SRCS} )

target_link_libraries (kpeoplewidgets
                       ${QT_QTCORE_LIBRARIES}
                       ${QT_QTGUI_LIBRARIES}
                       ${KDE4_KDEUI_LIBS}
                       kpeople
)

set_target_properties (kpeoplewidgets PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

if (MSVC)
    set_target_properties (kpeoplewidgets PROPERTIES
                           OUTPUT_NAME libkpeoplewidgets
    )
endif (MSVC)

# Install:
install (TARGETS kpeoplewidgets
         ${INSTALL_TARGETS_DEFAULT_ARGS}
)


install (FILES persondetailsview.h mergedialog.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople/widgets
         COMPONENT Devel
)

install (FILES persondetailsplugin.desktop
         DESTINATION
         ${SERVICETYPES_INSTALL_DIR}
)

