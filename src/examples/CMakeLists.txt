add_executable(contactlistwidgets contactlistwidgets.cpp)
target_link_libraries(contactlistwidgets Qt5::Core Qt5::Gui KPeople)

add_executable(personwidget personwidget.cpp)
target_link_libraries(personwidget Qt5::Core Qt5::Gui KPeople KPeopleWidgets)


# add_executable(loadcontact loadcontact.cpp)
# target_link_libraries(loadcontact Qt5::Core Qt5::Gui KF5::Abc KPeople)
