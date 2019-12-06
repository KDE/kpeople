qt5_wrap_cpp(kpeopledeclarative_SRCS ../widgets/actions.cpp)
add_library(KF5PeopleDeclarative SHARED
                    declarativepersondata.cpp
                    personactionsmodel.cpp
                    peopleqmlplugin.cpp ${kpeopledeclarative_SRCS})

target_link_libraries(KF5PeopleDeclarative
    Qt5::Qml
    KF5::PeopleWidgets # QAction
)

install(TARGETS KF5PeopleDeclarative DESTINATION ${QML_INSTALL_DIR}/org/kde/people)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/people)
