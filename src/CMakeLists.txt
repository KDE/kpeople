include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(declarative)
add_subdirectory(plugins)
add_subdirectory(widgets)


add_library (KPeople
    global.cpp
    metacontact.cpp
    persondata.cpp
    matchessolver.cpp
    match.cpp
    duplicatesfinder.cpp
    personsmodel.cpp
    personpluginmanager.cpp
    personmanager.cpp
    defaultcontactmonitor.cpp

    backends/contactmonitor.cpp
    backends/abstractpersonaction.cpp
    backends/abstractcontact.cpp
    backends/basepersonsdatasource.cpp
    backends/allcontactsmonitor.cpp
)
add_library(KF5::People ALIAS KPeople)

target_link_libraries (KPeople
                    PUBLIC
                        Qt5::Widgets #QAction
                    PRIVATE
                        Qt5::Sql
                        Qt5::DBus
                        KF5::Service
                        KF5::I18n
                        KF5::CoreAddons
)

set_target_properties (KPeople PROPERTIES VERSION ${KPEOPLE_VERSION_STRING} SOVERSION ${KPEOPLE_SOVERSION} EXPORT_NAME People)
target_include_directories (KPeople PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/kpeople>"
                                    INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KPeople>"
)

if (MSVC)
    set_target_properties (KPeople PROPERTIES
                           OUTPUT_NAME libkpeople
    )
endif (MSVC)

# Install:
install (TARGETS KPeople
         EXPORT KPeopleTargets
         ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

ecm_generate_headers(KPeople_CamelCase_HEADERS
    HEADER_NAMES
        PersonData
        PersonsModel
        Global
    REQUIRED_HEADERS KPeople_HEADERS
    PREFIX KPeople
)
install (FILES ${KPeople_CamelCase_HEADERS}
         DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KPeople/KPeople
         COMPONENT Devel
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kpeople/kpeople_export.h
  ${KPeople_HEADERS}
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KPeople/kpeople COMPONENT Devel
)

ecm_generate_headers(KPeopleBackend_CamelCase_HEADERS
    HEADER_NAMES
        AbstractContact
        AbstractPersonAction
        AllContactsMonitor
        BasePersonsDataSource
        ContactMonitor
    REQUIRED_HEADERS KPeopleBackend_HEADERS
    PREFIX KPeopleBackend
    RELATIVE backends
)
install (FILES ${KPeopleBackend_CamelCase_HEADERS}
         DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KPeople/KPeopleBackend
         COMPONENT Devel
)
install(FILES
  ${KPeopleBackend_HEADERS}
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KPeople/kpeoplebackend COMPONENT Devel
)

generate_export_header(KPeople EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/kpeople/kpeople_export.h BASE_NAME KPeople)

install (FILES
         kpeople_data_source.desktop
         kpeople_plugin.desktop
         DESTINATION
         ${SERVICETYPES_INSTALL_DIR}
)
