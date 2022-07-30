add_library(KF5PeopleWidgets)
add_library(KF5::PeopleWidgets ALIAS KF5PeopleWidgets)

set_target_properties(KF5PeopleWidgets PROPERTIES
    VERSION     ${KPEOPLE_VERSION}
    SOVERSION   ${KPEOPLE_SOVERSION}
    EXPORT_NAME PeopleWidgets
)

target_sources(KF5PeopleWidgets PRIVATE
    persondetailsdialog.cpp
    persondetailsview.cpp
    persondetailsdialog.cpp
    emaildetailswidget.cpp
    abstractfieldwidgetfactory.cpp
    actions.cpp

    mergedialog.cpp
    mergedelegate.cpp
)

ecm_qt_declare_logging_category(KF5PeopleWidgets
    HEADER kpeople_widgets_debug.h
    IDENTIFIER KPEOPLE_WIDGETS_LOG
    CATEGORY_NAME kf.people.widgets
    OLD_CATEGORY_NAMES kf5.kpeople.widgets
    DESCRIPTION "kpeople widgets"
    EXPORT KPEOPLE
)

ki18n_wrap_ui(KF5PeopleWidgets person-details-presentation.ui)

target_link_libraries (KF5PeopleWidgets
                    PUBLIC
                        KF5::People
                        Qt${QT_MAJOR_VERSION}::Widgets
                    PRIVATE
                        KF5::I18n
                        KF5::ItemViews
                        KF5::WidgetsAddons
                        KF5::PeopleBackend
                        KF5::CoreAddons
)

if (MSVC)
    set_target_properties (KF5PeopleWidgets PROPERTIES
                           OUTPUT_NAME libKPeopleWidgets
    )
endif (MSVC)

# Install:
install (TARGETS KF5PeopleWidgets
         EXPORT KPeopleTargets
         ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)

ecm_generate_headers(KPeopleWidgets_CamelCase_HEADERS
    HEADER_NAMES
            AbstractFieldWidgetFactory
            PersonDetailsView
            MergeDialog
            PersonDetailsDialog
            Actions
    REQUIRED_HEADERS KPeopleWidgets_HEADERS
    PREFIX KPeople/Widgets
)

install (FILES ${KPeopleWidgets_CamelCase_HEADERS}
         DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KPeople/KPeople/Widgets
         COMPONENT Devel
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kpeople/widgets/kpeoplewidgets_export.h
  ${KPeopleWidgets_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KPeople/kpeople/widgets COMPONENT Devel
)

generate_export_header(KF5PeopleWidgets EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/kpeople/widgets/kpeoplewidgets_export.h BASE_NAME KPeopleWidgets)

ecm_generate_pri_file(BASE_NAME KPeopleWidgets LIB_NAME KF5PeopleWidgets DEPS "widgets KPeople" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KPeople)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

# make available to ecm_add_qch in parent folder
set(KPeopleWidgets_QCH_SOURCES ${KPeopleWidgets_HEADERS} PARENT_SCOPE)
