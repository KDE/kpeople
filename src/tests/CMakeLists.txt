kde4_add_unit_test(duplicatestest duplicatestest.cpp)
target_link_libraries(duplicatestest kpeople
        ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS}
        ${NEPOMUK_LIBRARIES})
