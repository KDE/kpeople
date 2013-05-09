include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(declarative)

kde4_add_library (kpeople SHARED
    abstract-person-plugin.cpp
    persons-model-feature.cpp
    contact-item.cpp
    person-item.cpp
    persons-model.cpp
    persondata.cpp
    resource-watcher-service.cpp
    persons-presence-model.cpp
    duplicatesfinder.cpp
    matchessolver.cpp
    match.cpp

    ktp-translation-proxy.cpp
    person-actions.cpp

    plugins/im-plugin.cpp
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

install (FILES persons-model.h
               persondata.h
               kpeople_export.h
               persons-presence-model.h
               ktp-translation-proxy.h
               person-actions.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople
         COMPONENT Devel
)
