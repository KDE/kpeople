add_library (KF5PeopleBackend
    contactmonitor.cpp
    abstractcontact.cpp
    basepersonsdatasource.cpp
    allcontactsmonitor.cpp
    defaultcontactmonitor.cpp
    abstractpersonaction.cpp
)
add_library (KF5::PeopleBackend ALIAS KF5PeopleBackend)

target_link_libraries (KF5PeopleBackend
                    PUBLIC
                        Qt5::Widgets #needed for QAction, otherwise QtGui
)

target_include_directories (KF5PeopleBackend PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
                                    INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KPeople>"
)
set_target_properties (KF5PeopleBackend PROPERTIES VERSION ${KPEOPLE_VERSION_STRING} SOVERSION ${KPEOPLE_SOVERSION} EXPORT_NAME PeopleBackend)

install (TARGETS KF5PeopleBackend
         EXPORT KPeopleTargets
         ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
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
)
install (FILES ${KPeopleBackend_CamelCase_HEADERS}
         DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KPeople/KPeopleBackend
         COMPONENT Devel
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kpeoplebackend/kpeoplebackend_export.h
  ${KPeopleBackend_HEADERS}
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KPeople/kpeoplebackend COMPONENT Devel
)

generate_export_header(KF5PeopleBackend EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/kpeoplebackend/kpeoplebackend_export.h BASE_NAME KPeopleBackend)

# make available to ecm_add_qch in parent folder
set(KPeopleBackend_QCH_SOURCES ${KPeopleBackend_HEADERS} PARENT_SCOPE)
