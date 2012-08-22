kde4_add_executable(lovelypeople lovelypeople.cpp)
target_link_libraries(lovelypeople ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} kpeople)

find_package(KDeclarative REQUIRED)
if(KDeclarative_FOUND)
    qt4_add_resources(sweetpeople_SRCS sweetpeople.qrc)
    kde4_add_executable(sweetpeople sweetpeople.cpp ${sweetpeople_SRCS})
    target_link_libraries(sweetpeople ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
        ${QT_QTDECLARATIVE_LIBRARY} ${KDECLARATIVE_LIBRARIES})
endif(KDeclarative_FOUND)

kde4_add_executable(duplicates duplicates.cpp)
target_link_libraries(duplicates ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} kpeople)
