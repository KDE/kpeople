add_executable(contactlistwidgets contactlistwidgets.cpp)
target_link_libraries(contactlistwidgets Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Widgets KF6::People)

find_package(Qt${QT_MAJOR_VERSION}Quick)
if (Qt${QT_MAJOR_VERSION}Quick_FOUND)
    qt_add_resources(contactlistqml_SRCS contactlistqml.qrc)
    add_executable(contactlistqml contactlistqml.cpp ${contactlistqml_SRCS})
    target_link_libraries(contactlistqml Qt${QT_MAJOR_VERSION}::Quick)
endif()

add_executable(personwidget personwidget.cpp)
target_link_libraries(personwidget Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Widgets KF6::People KF6::PeopleWidgets)


# add_executable(loadcontact loadcontact.cpp)
# target_link_libraries(loadcontact Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Gui KF6::People)

add_executable(duplicates duplicates.cpp)
target_link_libraries(duplicates KF6::People KF6::CoreAddons KF6::PeopleBackend)

add_executable(duplicatesdialog duplicatesdialog.cpp)
target_link_libraries(duplicatesdialog KF6::PeopleWidgets Qt${QT_MAJOR_VERSION}::Widgets)
