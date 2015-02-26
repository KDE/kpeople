# add_subdirectory(plugins)

set (kpeople_widgets_SRCS persondetailsdialog.cpp
    persondetailsview.cpp
    persondetailsdialog.cpp
    plugins/emaildetailswidget.cpp
    abstractfieldwidgetfactory.cpp
    actions.cpp

    mergedialog.cpp
    mergedelegate.cpp
)

qt5_wrap_ui (kpeople_widgets_SRCS person-details-presentation.ui)

add_library (KPeopleWidgets SHARED ${kpeople_widgets_SRCS} )
add_library (KF5::PeopleWidgets ALIAS KPeopleWidgets)

target_link_libraries (KPeopleWidgets
                    PUBLIC
                        KPeople
                        Qt5::Widgets
                    PRIVATE
                        KF5::I18n
                        KF5::Service
                        KF5::ItemViews
                        KF5::WidgetsAddons
                        KPeopleBackend
)

set_target_properties (KPeopleWidgets PROPERTIES VERSION ${KPEOPLE_VERSION_STRING} SOVERSION ${KPEOPLE_SOVERSION} EXPORT_NAME PeopleWidgets)

if (MSVC)
    set_target_properties (KPeopleWidgets PROPERTIES
                           OUTPUT_NAME libKPeopleWidgets
    )
endif (MSVC)

# Install:
install (TARGETS KPeopleWidgets
         EXPORT KPeopleTargets
         ${INSTALL_TARGETS_DEFAULT_ARGS}
)

install (FILES

         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople/widgets
         COMPONENT Devel
)

ecm_generate_headers(KPeopleWidgets_CamelCase_HEADERS
    HEADER_NAMES
            AbstractFieldWidgetFactory
            PersonDetailsView
            MergeDialog
            PersonDetailsDialog
            Actions
    REQUIRED_HEADERS KPeopleWidgets_HEADERS
    PREFIX KPeople
)

install (FILES ${KPeopleWidgets_CamelCase_HEADERS}
         DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KPeople/KPeople/Widgets
         COMPONENT Devel
)
install(FILES
  ${KPeopleWidgets_HEADERS}
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KPeople/kpeople/widgets COMPONENT Devel
)

install (FILES persondetailsplugin.desktop
         DESTINATION
            ${SERVICETYPES_INSTALL_DIR}
)

ecm_generate_pri_file(BASE_NAME KPeopleWidgets LIB_NAME KF5ConfigCore DEPS "widgets KPeople" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KPeople)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
