ecm_qt_declare_logging_category(KF5People_akonadi_debug_SRCS HEADER kpeople_akonadi_plugin_debug.h IDENTIFIER KPEOPLE_AKONADI_PLUGIN_LOG CATEGORY_NAME kf5.kpeople.plugin.akonadi)

add_library(akonadi_kpeople_plugin MODULE akonadidatasource.cpp ${KF5People_akonadi_debug_SRCS})

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

