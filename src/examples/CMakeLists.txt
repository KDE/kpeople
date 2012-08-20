kde4_add_executable(lovelypeople lovelypeople.cpp)
target_link_libraries(lovelypeople ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} kpeople)

kde4_add_executable(duplicates duplicates.cpp)
target_link_libraries(duplicates ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} kpeople)
