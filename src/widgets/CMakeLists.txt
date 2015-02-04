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

if (Baloo_FOUND)
    set(kpeople_widgets_SRCS
        ${kpeople_widgets_SRCS}
        personemailview.cpp
        plugins/emails.cpp
        plugins/emaillistviewdelegate.cpp
        plugins/emaillistmodel.cpp)
endif()

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
                        KPeopleBackends
)

if (Baloo_FOUND)
    target_link_libraries(KPeopleWidgets KF5::Baloo)
endif()

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
            PersonEmailView
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

