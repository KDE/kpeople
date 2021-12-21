include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(declarative)
add_subdirectory(widgets)
add_subdirectory(backends)

add_library(KF5People)
add_library(KF5::People ALIAS KF5People)

set_target_properties (KF5People PROPERTIES
    VERSION     ${KPEOPLE_VERSION}
    SOVERSION   ${KPEOPLE_SOVERSION}
    EXPORT_NAME People
)

ecm_qt_declare_logging_category(KF5People
    HEADER kpeople_debug.h
    IDENTIFIER KPEOPLE_LOG
    CATEGORY_NAME kf.people.core
    OLD_CATEGORY_NAMES kf5.kpeople
    DESCRIPTION "kpeople lib"
    EXPORT KPEOPLE
)

target_sources(KF5People PRIVATE
    global.cpp
    metacontact.cpp
    persondata.cpp
    matchessolver.cpp
    match.cpp
    duplicatesfinder.cpp
    personsmodel.cpp
    personpluginmanager.cpp
    personmanager.cpp
    personssortfilterproxymodel.cpp
    resources.qrc
)

target_link_libraries (KF5People
                    PUBLIC
                        Qt${QT_MAJOR_VERSION}::Gui
                    PRIVATE
                        Qt${QT_MAJOR_VERSION}::Sql
                        KF5::I18n
                        KF5::CoreAddons
                        KF5::PeopleBackend
)

if(TARGET Qt${QT_MAJOR_VERSION}::DBus)
    target_link_libraries(KF5People PRIVATE Qt${QT_MAJOR_VERSION}::DBus)
endif()

target_include_directories (KF5People PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/kpeople>"
                                    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KPeople>"
)

if (MSVC)
    set_target_properties (KF5People PROPERTIES
                           OUTPUT_NAME libkpeople
    )
endif (MSVC)

# Install:
install (TARGETS KF5People
         EXPORT KPeopleTargets
         ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

ecm_generate_headers(KPeople_CamelCase_HEADERS
    HEADER_NAMES
        PersonData
        PersonPluginManager
        PersonsModel
        PersonsSortFilterProxyModel
        Global
    REQUIRED_HEADERS KPeople_HEADERS
    PREFIX KPeople
)
install (FILES ${KPeople_CamelCase_HEADERS}
         DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KPeople/KPeople
         COMPONENT Devel
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kpeople/kpeople_export.h
  ${KPeople_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KPeople/kpeople COMPONENT Devel
)

install(FILES personmanager_p.h DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KPeople/kpeopleprivate)

generate_export_header(KF5People EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/kpeople/kpeople_export.h BASE_NAME KPeople)


if(BUILD_QCH)
    ecm_add_qch(
        KF5People_QCH
        NAME KPeople
        BASE_NAME KF5People
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KPeople_HEADERS}
            ${KPeopleWidgets_QCH_SOURCES}
            ${KPeopleBackend_QCH_SOURCES}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Gui_QCH
            Qt5Widgets_QCH
        BLANK_MACROS
            KPEOPLE_EXPORT
            KPEOPLE_DEPRECATED
            KPEOPLE_DEPRECATED_EXPORT
            KPEOPLEWIDGETS_EXPORT
            KPEOPLEWIDGETS_DEPRECATED
            KPEOPLEWIDGETS_DEPRECATED_EXPORT
            KPEOPLEBACKEND_EXPORT
            KPEOPLEBACKEND_DEPRECATED
            KPEOPLEBACKEND_DEPRECATED_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

install (FILES
         kpeople_data_source.desktop
         kpeople_plugin.desktop
         DESTINATION
         ${KDE_INSTALL_KSERVICETYPESDIR}
)

ecm_qt_install_logging_categories(
    EXPORT KPEOPLE
    FILE kpeople.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

ecm_generate_pri_file(BASE_NAME KPeople LIB_NAME KF5People DEPS "gui" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KPeople)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if (ANDROID)
    install(FILES KF5People-android-dependencies.xml DESTINATION ${KDE_INSTALL_LIBDIR})
endif()
