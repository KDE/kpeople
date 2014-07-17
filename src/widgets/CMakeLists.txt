#add_subdirectory(plugins)

include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set (kpeople_widgets_SRCS
    plugins/chatlistviewdelegate.cpp
    plugins/events.cpp
    personeventview.cpp
    plugins/chat.cpp
    personchatview.cpp
    plugins/note.cpp
    personnoteview.cpp
    plugins/emaillistviewdelegate.cpp
    plugins/emaillistmodel.cpp
    personemailview.cpp
    plugins/emails.cpp
    persondetailsdialog.cpp
    persondetailsview.cpp
    persondetailsdialog.cpp
    plugins/emaildetailswidget.cpp
    abstractfieldwidgetfactory.cpp

#     mergedialog.cpp
#     mergedelegate.cpp
)

kde4_add_ui_files (kpeople_widgets_SRCS
                   person-details-presentation.ui
)

kde4_add_library (kpeoplewidgets SHARED ${kpeople_widgets_SRCS} )

target_link_libraries (kpeoplewidgets
                       ${QT_QTCORE_LIBRARIES}
                       ${QT_QTGUI_LIBRARIES}
                       ${KDE4_KDEUI_LIBS}
                       ${KDEPIMLIBS_KABC_LIBS}
                       ${KDEPIMLIBS_AKONADI_LIBS}
                       ${KDEPIMLIBS_KCALCORE_LIBS}
                       ${KDEPIMLIBS_KMIME_LIBS}
                       ${BALOO_LIBRARIES}   
                       ${LibKFbAPI_LIBRARY}
                       ${TELEPATHY_QT4_LIBRARIES}
                       ${KTP_LIBRARIES}
                       ${KTP_LOGGER_LIBRARIES}
                       kpeople
)

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
            personeventview.h
            persondetailsview.h
            personemailview.h
            personnoteview.h
            personchatview.h
#             mergedialog.h
            persondetailsdialog.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople/widgets
         COMPONENT Devel
)

install (FILES persondetailsplugin.desktop
	 personemailplugin.desktop
         DESTINATION
         ${SERVICETYPES_INSTALL_DIR}
)

