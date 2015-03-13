add_library(akonadi_kpeople_plugin MODULE akonadidatasource.cpp)

target_link_libraries (akonadi_kpeople_plugin
    ${KDEPIMLIBS_AKONADI_LIBS}
    KF5::Contacts
    KF5::People
)

# Install:
install (TARGETS akonadi_kpeople_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}
)

install (FILES akonadi_kpeople_plugin.desktop
         DESTINATION ${SERVICES_INSTALL_DIR}
)

