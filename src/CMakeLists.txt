include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(declarative)

kde4_add_library (kpeople SHARED
    persons-model-contact-item.cpp
    persons-model-item.cpp
    persons-model.cpp
    nepomuk-tp-channel-delegate.cpp
    person-cache.cpp
    personactions.cpp

    duplicatesfinder.cpp
    matchessolver.cpp
    match.cpp
)

target_link_libraries (kpeople
                       ${QT_QTCORE_LIBRARIES}
                       ${KDE4_KIO_LIBS}
                       ${NEPOMUK_LIBRARIES}
                       ${NEPOMUK_QUERY_LIBRARIES}
                       ${TELEPATHY_QT4_LIBRARIES}
                       ${KTP_LIBRARIES}
                       nepomukcore
#                        ontologies

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
         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople
         COMPONENT Devel
)
