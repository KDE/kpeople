add_library(KPeopleDeclarative SHARED
                    declarativepersondata.cpp
                    personactionsmodel.cpp
                    peopleqmlplugin.cpp)

target_link_libraries(KPeopleDeclarative
    Qt5::Qml KF5::People
    Qt5::Widgets # QAction
)

install(TARGETS KPeopleDeclarative DESTINATION ${QML_INSTALL_DIR}/org/kde/people)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/people)
