include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(declarative)
add_subdirectory(plugins)
add_subdirectory(widgets)


add_library (KPeople
      global.cpp
      metacontact.cpp
    abstractpersonaction.cpp
    persondata.cpp
    matchessolver.cpp
    match.cpp
    duplicatesfinder.cpp
    personsmodel.cpp
    personpluginmanager.cpp
    personmanager.cpp
    basepersonsdatasource.cpp
    allcontactsmonitor.cpp
    contactmonitor.cpp
    defaultcontactmonitor.cpp
)
add_library(KF5::People ALIAS KPeople)

target_link_libraries (KPeople
                    PUBLIC
                        KF5::Contacts
                    PRIVATE
                        Qt5::Sql
                        Qt5::DBus
                        KF5::KIOWidgets
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
        AbstractPersonAction
        AllContactsMonitor
        BasePersonsDataSource
        ContactMonitor
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

generate_export_header(KPeople EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/kpeople/kpeople_export.h BASE_NAME KPeople)

install (FILES
         kpeople_data_source.desktop
         kpeople_plugin.desktop
         DESTINATION
         ${SERVICETYPES_INSTALL_DIR}
)
