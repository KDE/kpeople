kde4_add_executable(contactlistwidgets contactlistwidgets.cpp)
target_link_libraries(contactlistwidgets ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} kpeople)

kde4_add_executable(loadcontact loadcontact.cpp)
target_link_libraries(loadcontact ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDEPIMLIBS_KABC_LIBS} kpeople)
