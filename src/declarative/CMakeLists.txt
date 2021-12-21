add_library(KF5PeopleDeclarative SHARED)

target_sources(KF5PeopleDeclarative PRIVATE
    declarativepersondata.cpp
    personactionsmodel.cpp
    peopleqmlplugin.cpp

    # automoc the ActionType enum
    ../widgets/actions.h
)

target_link_libraries(KF5PeopleDeclarative
    Qt${QT_MAJOR_VERSION}::Qml
    KF5::PeopleWidgets # QAction
)

install(TARGETS KF5PeopleDeclarative DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/people)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/people)
