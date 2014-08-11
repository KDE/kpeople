#add_subdirectory(plugins)

include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set (kpeople_widgets_SRCS
    plugins/filesviewdelegate.cpp
    plugins/files.cpp
    personfilesview.cpp
    plugins/postviewdelegate.cpp
    plugins/posts.cpp
    personpostview.cpp
    plugins/eventviewdelegate.cpp
    plugins/events.cpp
    personeventview.cpp
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
                   plugins/filelist.ui
)

kde4_add_library (kpeoplewidgets SHARED ${kpeople_widgets_SRCS} )

target_link_libraries (kpeoplewidgets
                       ${QT_QTCORE_LIBRARIES}
                       ${QT_QTGUI_LIBRARIES}
                       ${QT_QTUITOOLS_LIBRARIES}
                       ${KDE4_KDEUI_LIBS}
                       ${KDEPIMLIBS_KABC_LIBS}
                       ${KDEPIMLIBS_AKONADI_LIBS}
                       ${KDEPIMLIBS_KCALCORE_LIBS}
                       ${KDEPIMLIBS_KMIME_LIBS}
                       ${BALOO_LIBRARIES}   
                       ${LibKFbAPI_LIBRARY}
                       ${LibKGAPI2_LIBRARY}
                       ${KDEPIMLIBS_AKONADI_SOCIALUTILS_LIBS}
                       ${QT_QTSQL_LIBRARIES}
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
            personfilesview.h
            personeventview.h
            persondetailsview.h
            personemailview.h
            personnoteview.h
            personchatview.h
            personpostview.h
#             mergedialog.h
            persondetailsdialog.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople/widgets
         COMPONENT Devel
)
INSTALL (FILES plugins/filelist.ui DESTINATION ${DATA_INSTALL_DIR}/kpeople/)
install (FILES persondetailsplugin.desktop
         DESTINATION
         ${SERVICETYPES_INSTALL_DIR}
)

