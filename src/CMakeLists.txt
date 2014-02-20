include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

if (ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif (ENABLE_EXAMPLES)
if (KDE4_BUILD_TESTS)
  add_subdirectory(autotests)
endif (KDE4_BUILD_TESTS)
# add_subdirectory(declarative)
add_subdirectory(plugins)
add_subdirectory(widgets)


kde4_add_library (kpeople    SHARED
      global.cpp
      metacontact.cpp
    abstractpersonaction.cpp
    persondata.cpp
#     matchessolver.cpp
#     match.cpp
    personsmodel.cpp
    personpluginmanager.cpp
    personmanager.cpp
    basepersonsdatasource.cpp
    allcontactsmonitor.cpp
    contactmonitor.cpp
    defaultcontactmonitor.cpp
)

target_link_libraries (kpeople
                       ${QT_QTCORE_LIBRARIES}
                       ${QT_QTSQL_LIBRARIES}
                       ${KDE4_KIO_LIBS}
                       ${KDEPIMLIBS_AKONADI_LIBS} #can be removed when we remove temp bodge above
                       ${KDEPIMLIBS_KABC_LIBS}
)

set_target_properties (kpeople PROPERTIES VERSION ${KPEOPLE_VERSION} SOVERSION ${KPEOPLE_SOVERSION})

if (MSVC)
    set_target_properties (kpeople PROPERTIES
                           OUTPUT_NAME libkpeople
    )
endif (MSVC)

# Install:
install (TARGETS kpeople
         ${INSTALL_TARGETS_DEFAULT_ARGS}
)

install (FILES
            global.h
            personsmodel.h
            persondata.h
            kpeople_export.h
            abstractpersonaction.h
            basepersonsdatasource.h
            allcontactsmonitor.h
            contactmonitor.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople/
         COMPONENT Devel
)

install (FILES
            KPeople/AbstractPersonAction
            KPeople/AllContactsMonitor
            KPeople/BasePersonsDataSource
            KPeople/ContactMonitor
            KPeople/PersonData
            KPeople/PersonsModel
         DESTINATION ${INCLUDE_INSTALL_DIR}/KPeople
         COMPONENT Devel
)

install (FILES
         kpeople_data_source.desktop
         kpeople_plugin.desktop
         DESTINATION
         ${SERVICETYPES_INSTALL_DIR}
)
