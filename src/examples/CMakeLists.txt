kde4_add_executable(contactlistwidgets contactlistwidgets.cpp)
target_link_libraries(contactlistwidgets ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} kpeople)

kde4_add_executable(personwidget personwidget.cpp)
target_link_libraries(personwidget ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} kpeople kpeoplewidgets)


# kde4_add_executable(loadcontact loadcontact.cpp)
# target_link_libraries(loadcontact ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDEPIMLIBS_KABC_LIBS} kpeople)
