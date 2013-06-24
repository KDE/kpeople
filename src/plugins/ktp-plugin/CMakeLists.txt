set (im_persons_data_source_SRCS
     impersonsdatasourceplugin.cpp
     impersonsdatasource.cpp
)

kde4_add_plugin (im_persons_data_source_plugin
                 ${im_persons_data_source_SRCS}
)

target_link_libraries (im_persons_data_source_plugin
    ${QT_LIBRARIES}
    ${TELEPATHY_QT4_LIBRARIES}
    ${KTP_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${NEPOMUK_CORE_LIBRARY}
    kpeople
)

# Install:
install (TARGETS im_persons_data_source_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}
)

install (FILES im_persons_data_source_plugin.desktop
         DESTINATION ${SERVICES_INSTALL_DIR}
)


install (FILES impersonsdatasource.h
         DESTINATION ${INCLUDE_INSTALL_DIR}/kpeople
         COMPONENT Devel
)


set (ktp_kpeople_plugin_SRCS
     impersonsdatasource.cpp
     implugin.cpp
)

kde4_add_plugin (ktp_kpeople_plugin
                 ${ktp_kpeople_plugin_SRCS}
)

target_link_libraries (ktp_kpeople_plugin
    ${QT_LIBRARIES}
    ${TELEPATHY_QT4_LIBRARIES}
    ${KTP_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${NEPOMUK_CORE_LIBRARY}
    kpeople
)

# Install:
install (TARGETS ktp_kpeople_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}
)

install (FILES ktp_kpeople_plugin.desktop
         DESTINATION ${SERVICES_INSTALL_DIR}
)
