add_library(KF5PeopleDeclarative SHARED)

target_sources(KF5PeopleDeclarative PRIVATE
    declarativepersondata.cpp
    personactionsmodel.cpp
    peopleqmlplugin.cpp
    avatarimageprovider.cpp

    # automoc the ActionType enum
    ../widgets/actions.h
)

ecm_qt_declare_logging_category(KF5PeopleDeclarative
    HEADER kpeopledeclarative_debug.h
    IDENTIFIER KPEOPLE_DECLARATIVE_LOG
    CATEGORY_NAME kf.people.declarative
    DESCRIPTION "KPeople QML plugin"
    EXPORT KPEOPLE_DECLARATIVE
)

target_link_libraries(KF5PeopleDeclarative
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Quick
    KF5::PeopleWidgets # QAction
    KF5PeopleBackend
)

install(TARGETS KF5PeopleDeclarative DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/people)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/people)
