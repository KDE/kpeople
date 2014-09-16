# add_subdirectory(plugins)

include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set (kpeople_widgets_SRCS persondetailsdialog.cpp
    persondetailsview.cpp
    persondetailsdialog.cpp
    plugins/emaildetailswidget.cpp
    abstractfieldwidgetfactory.cpp

#     mergedialog.cpp
#     mergedelegate.cpp
)

if (${Baloo_FOUND})
    set(kpeople_widgets_SRCS
        ${kpeople_widgets_SRCS}
        personemailview.cpp
        plugins/emails.cpp
        plugins/emaillistviewdelegate.cpp
        plugins/emaillistmodel.cpp)
endif()

kde4_add_ui_files (kpeople_widgets_SRCS
                   person-details-presentation.ui
)

kde4_add_library (kpeoplewidgets SHARED ${kpeople_widgets_SRCS} )

target_link_libraries (kpeoplewidgets
                       ${QT_QTCORE_LIBRARIES}
                       ${QT_QTGUI_LIBRARIES}
                       ${KDE4_KDEUI_LIBS}
                       ${KDEPIMLIBS_KABC_LIBS}
                       ${KDEPIMLIBS_KMIME_LIBS}
                       ${KDEPIMLIBS_AKONADI_LIBS}
                       kpeople
)

if (${Baloo_FOUND})
    target_link_libraries(kpeoplewidgets ${BALOO_LIBRARIES})
endif()

set_target_properties (kpeoplewidgets PROPERTIES VERSION ${KPEOPLE_LIBVERSION} SOVERSION ${KPEOPLE_SOVERSION})

if (MSVC)
    set_target_properties (kpeoplewidgets PROPERTIES
                           OUTPUT_NAME libkpeoplewidgets
    )
endif (MSVC)

# Install:
install (TARGETS kpeoplewidgets
         ${INSTALL_TARGETS_DEFAULT_ARGS}
)


install (FILES
            abstractfieldwidgetfactory.h
            persondetailsview.h
#             mergedialog.h
            persondetailsdialog.h
            personemailview.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople/widgets
         COMPONENT Devel
)

install (FILES persondetailsplugin.desktop
         DESTINATION
         ${SERVICETYPES_INSTALL_DIR}
)

