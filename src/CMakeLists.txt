include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(declarative)
add_subdirectory(plugins)
add_subdirectory(widgets)
add_subdirectory(backends)

add_library (KF5People
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
)
add_library(KF5::People ALIAS KF5People)

target_link_libraries (KF5People
                    PUBLIC
                        Qt5::Gui
                    PRIVATE
                        Qt5::Sql
                        Qt5::DBus
                        KF5::I18n
                        KF5::CoreAddons
                        KF5::PeopleBackend
                        KF5::Service
)

set_target_properties (KF5People PROPERTIES VERSION ${KPEOPLE_VERSION_STRING} SOVERSION ${KPEOPLE_SOVERSION} EXPORT_NAME People)
target_include_directories (KF5People PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/kpeople>"
                                    INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KPeople>"
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

generate_export_header(KF5People EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/kpeople/kpeople_export.h BASE_NAME KPeople)

install (FILES
         kpeople_data_source.desktop
         kpeople_plugin.desktop
         DESTINATION
         ${SERVICETYPES_INSTALL_DIR}
)

ecm_generate_pri_file(BASE_NAME KPeople LIB_NAME KF5ConfigCore DEPS "gui" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KPeople)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
