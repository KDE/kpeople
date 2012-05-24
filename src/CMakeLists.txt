include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set (kpeople_SRCS kpeople-manager.cpp
     persons-model-contact-item.cpp
     persons-model-item.cpp
     persons-model.cpp
     nepomuk-tp-channel-delegate.cpp
     person-cache.cpp
     person-cache-item.cpp
     resource-watcher-service.cpp
     tree-node.cpp
)

set (kpeople_INST_HDRS
#      kpeople_export.h
     nepomuk-tp-channel-delegate.h
     person-cache.h
#      person-cache-item.h
     person-cache-item-set_p.h
     persons-model-contact-item.h
     persons-model-item.h
     persons-model.h
     kpeople-manager.h
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

