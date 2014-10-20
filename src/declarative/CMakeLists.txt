add_library(KPeopleDeclarative SHARED
                    declarativepersonsmodel.cpp
                    declarativepersondata.cpp
                    peopleqmlplugin.cpp)

target_link_libraries(KPeopleDeclarative Qt5::Core Qt5::Gui Qt5::Declarative KPeople)

install(TARGETS KPeopleDeclarative DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/people)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/people)
