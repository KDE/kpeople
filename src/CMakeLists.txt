include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(examples)
add_subdirectory(autotests)
add_subdirectory(declarative)
add_subdirectory(plugins)
add_subdirectory(widgets)


kde4_add_library (kpeople SHARED
    abstractpersonplugin.cpp
    personsmodelfeature.cpp
    contactitem.cpp
    datasourcewatcher.cpp
    personitem.cpp
    personsmodel.cpp
    persondata.cpp
    resourcewatcherservice.cpp
    duplicatesfinder.cpp
    matchessolver.cpp
    match.cpp
    personactionsmodel.cpp
    personpluginmanager.cpp
    basepersonsdatasource.cpp
    plugins/ktp-plugin/impersonsdatasource.cpp

    ktptranslationproxy.cpp
)

target_link_libraries (kpeople
                       ${QT_QTCORE_LIBRARIES}
                       ${KDE4_KIO_LIBS}
                       ${NEPOMUK_CORE_LIBRARY}
                       ${TELEPATHY_QT4_LIBRARIES}
                       ${KTP_LIBRARIES}
)

set_target_properties (kpeople PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

if (MSVC)
    set_target_properties (kpeople PROPERTIES
                           OUTPUT_NAME libkpeople
    )
endif (MSVC)

# Install:
install (TARGETS kpeople
         ${INSTALL_TARGETS_DEFAULT_ARGS}
)

install (FILES personsmodel.h
               persondata.h
               kpeople_export.h
               ktptranslationproxy.h
               personactionsmodel.h
               abstractpersonplugin.h
               basepersonsdatasource.h
               personpluginmanager.h

         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople
         COMPONENT Devel
)

install (FILES
         kpeople_data_source.desktop
         kpeople_plugin.desktop
         DESTINATION
         ${SERVICETYPES_INSTALL_DIR}
)
