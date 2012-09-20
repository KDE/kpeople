kde4_add_unit_test(duplicatestest duplicatestest.cpp)
target_link_libraries(duplicatestest kpeople
        ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS}
        nepomukcore)

kde4_add_unit_test(personsmodeltest personsmodeltest.cpp)
target_link_libraries(personsmodeltest kpeople
        ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS}
        ${NEPOMUK_LIBRARIES})

kde4_add_unit_test(persondatatest persondatatest.cpp)
target_link_libraries(persondatatest kpeople
        ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS}
        nepomukcore)