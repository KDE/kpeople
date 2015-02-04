add_library (KPeopleBackend
    contactmonitor.cpp
    abstractcontact.cpp
    basepersonsdatasource.cpp
    allcontactsmonitor.cpp
    defaultcontactmonitor.cpp
    abstractpersonaction.cpp
)
add_library (KF5::PeopleBackend ALIAS KPeopleBackend)

target_link_libraries (KPeopleBackend
                    PUBLIC
                        Qt5::Widgets #needed for QAction, otherwise QtGui
)

target_include_directories (KPeopleBackend INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KPeople>")
set_target_properties (KPeopleBackend PROPERTIES VERSION ${KPEOPLE_VERSION_STRING} SOVERSION ${KPEOPLE_SOVERSION} EXPORT_NAME PeopleBackend)

install (TARGETS KPeopleBackend
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
  ${KPeopleBackend_HEADERS}
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KPeople/kpeoplebackend COMPONENT Devel
)
