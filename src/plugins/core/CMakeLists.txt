set (email_kpeople_plugin_SRCS
     email-plugin.cpp
)

kde4_add_plugin (email_kpeople_plugin
                 ${email_kpeople_plugin_SRCS}
)

target_link_libraries (email_kpeople_plugin
    ${QT_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    kpeople
)

# Install:
install (TARGETS email_kpeople_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}
)

install (FILES email_kpeople_plugin.desktop
         DESTINATION ${SERVICES_INSTALL_DIR}
)

