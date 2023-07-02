add_library(KF6PeopleDeclarative SHARED)

target_sources(KF6PeopleDeclarative PRIVATE
    declarativepersondata.cpp
    personactionsmodel.cpp
    peopleqmlplugin.cpp
    avatarimageprovider.cpp
)

ecm_qt_declare_logging_category(KF6PeopleDeclarative
    HEADER kpeopledeclarative_debug.h
    IDENTIFIER KPEOPLE_DECLARATIVE_LOG
    CATEGORY_NAME kf.people.declarative
    DESCRIPTION "KPeople QML plugin"
    EXPORT KPEOPLE_DECLARATIVE
)

target_link_libraries(KF6PeopleDeclarative
    Qt6::Qml
    Qt6::Quick
    KF6PeopleBackend
    KF6::People
)

install(TARGETS KF6PeopleDeclarative DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/people)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/people)
