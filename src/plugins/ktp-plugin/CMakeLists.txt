set (im_persons_data_source_SRCS
     im-persons-data-source-plugin.cpp
     im-persons-data-source.cpp
)

kde4_add_plugin (im_persons_data_source_plugin
                 ${im_persons_data_source_SRCS}
)

target_link_libraries (im_persons_data_source_plugin
    ${QT_LIBRARIES}
    ${TELEPATHY_QT4_LIBRARIES}
    ${KTP_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    kpeople
)

# Install:
install (TARGETS im_persons_data_source_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}
)

install (FILES im_persons_data_source_plugin.desktop
         DESTINATION ${SERVICES_INSTALL_DIR}
)




set (ktp_kpeople_plugin_SRCS
     im-persons-data-source.cpp
     im-plugin.cpp
)

kde4_add_plugin (ktp_kpeople_plugin
                 ${ktp_kpeople_plugin_SRCS}
)

target_link_libraries (ktp_kpeople_plugin
    ${QT_LIBRARIES}
    ${TELEPATHY_QT4_LIBRARIES}
    ${KTP_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    kpeople
)

# Install:
install (TARGETS ktp_kpeople_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}
)

install (FILES ktp_kpeople_plugin.desktop
         DESTINATION ${SERVICES_INSTALL_DIR}
)


