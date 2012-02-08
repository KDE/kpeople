include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set (kpeople_SRCS digikam-persons-model.cpp
     digikam-person-cache-item-facet.cpp
     digikam-persons-manager.cpp
     nepomuk-tp-channel-delegate.cpp
     pim-persons-model.cpp
     abstract-persons-manager.cpp
#      pim-persons-manager.cpp
     resource-watcher-service.cpp
     im-persons-manager.cpp
     im-person-cache-item-facet.cpp
     im-persons-model.cpp
#      basic-person-cache-item-facet.cpp
     person-cache.cpp
     person-cache-item.cpp
     person-cache-item-set.cpp
)

set (kpeople_INST_HDRS
     abstract-persons-manager.h
     digikam-persons-manager.h
     digikam-persons-model.h
     kpeople_export.h
     im-persons-manager.h
     im-persons-model.h
     basic-person-cache-item-facet.h
     nepomuk-tp-channel-delegate.h
     person-cache.h
     person-cache-item.h
     person-cache-item-set.h
     person-cache-item-set_p.h
)

set (kpeople_PRETTY_HDRS

)

kde4_add_library (kpeople
                  SHARED
                  ${kpeople_SRCS}
)

target_link_libraries (kpeople
                       ${QT_QTCORE_LIBRARIES}
                       ${KDE4_KIO_LIBS}
                       ${NEPOMUK_LIBRARIES}
                       ${NEPOMUK_QUERY_LIBRARIES}
                       ${TELEPATHY_QT4_LIBRARIES}
                       ${KTP_LIBRARIES}
                       nepomukdatamanagement
#                        ontologies

)

set_target_properties (kpeople PROPERTIES
                       VERSION ${GENERIC_LIB_VERSION}
                       SOVERSION ${GENERIC_LIB_SOVERSION}
)

if (MSVC)
    set_target_properties (kpeople PROPERTIES
                           OUTPUT_NAME libkpeople
    )
endif (MSVC)

# Install:
install (TARGETS kpeople
         ${INSTALL_TARGETS_DEFAULT_ARGS}
)

install (FILES ${kpeople_INST_HDRS}
         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople
         COMPONENT Devel
)

install (FILES ${kpeople_PRETTY_HDRS}
         DESTINATION ${INCLUDE_INSTALL_DIR}/KDE/KPeople
         COMPONENT Devel
)

