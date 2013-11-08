set (akonadi_source_plugin_SRCS
     akonadidatasource.cpp
)

kde4_add_plugin (akonadi_kpeople_plugin
                 ${akonadi_source_plugin_SRCS}
)

target_link_libraries (akonadi_kpeople_plugin
    ${QT_LIBRARIES}
    ${KDEPIMLIBS_AKONADI_LIBS}
    ${KDEPIMLIBS_KABC_LIBS}
    kpeople
)

# Install:
install (TARGETS akonadi_kpeople_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}
)

install (FILES akonadi_kpeople_plugin.desktop
         DESTINATION ${SERVICES_INSTALL_DIR}
)

