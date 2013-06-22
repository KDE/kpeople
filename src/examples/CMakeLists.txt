kde4_add_executable(contactlistwidgets contactlistwidgets.cpp)
target_link_libraries(contactlistwidgets ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} kpeople)

find_package(KDeclarative REQUIRED)
qt4_add_resources(contactlistqml_SRCS contactlistqml.qrc)
kde4_add_executable(contactlistqml contactlistqml.cpp ${contactlistqml_SRCS})
target_link_libraries(contactlistqml ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY} ${KDECLARATIVE_LIBRARIES} ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS})

kde4_add_executable(duplicates duplicates.cpp)
target_link_libraries(duplicates ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBRARY} kpeople)

kde4_add_executable(personwidget personwidget.cpp)
target_link_libraries(personwidget ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} kpeople)
