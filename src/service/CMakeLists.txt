add_executable(kpeoplelookupservice
    service.cpp
    main.cpp
)

target_link_libraries(kpeoplelookupservice
    KF5::People
    KF5::PeopleBackend
    Qt5::DBus
)

install (TARGETS kpeoplelookupservice
         ${INSTALL_TARGETS_DEFAULT_ARGS}
)

configure_file(org.kde.KPeople.service.in
               ${CMAKE_CURRENT_BINARY_DIR}/org.kde.KPeople.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.KPeople.service
        DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})
