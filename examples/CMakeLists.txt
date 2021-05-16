add_executable(contactlistwidgets contactlistwidgets.cpp)
target_link_libraries(contactlistwidgets Qt5::Core Qt5::Widgets KF5::People)

find_package(Qt5Quick)
if (Qt5Quick_FOUND)
    qt_add_resources(contactlistqml_SRCS contactlistqml.qrc)
    add_executable(contactlistqml contactlistqml.cpp ${contactlistqml_SRCS})
    target_link_libraries(contactlistqml Qt5::Quick)
endif()

add_executable(personwidget personwidget.cpp)
target_link_libraries(personwidget Qt5::Core Qt5::Widgets KF5::People KF5::PeopleWidgets)


# add_executable(loadcontact loadcontact.cpp)
# target_link_libraries(loadcontact Qt5::Core Qt5::Gui KF5::People)

add_executable(duplicates duplicates.cpp)
target_link_libraries(duplicates KF5::People KF5::CoreAddons KF5::PeopleBackend)

add_executable(duplicatesdialog duplicatesdialog.cpp)
target_link_libraries(duplicatesdialog KF5::PeopleWidgets Qt5::Widgets)
