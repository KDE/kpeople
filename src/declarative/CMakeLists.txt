project(kpeople-declarative)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
)

kde4_add_library(kpeopledeclarative SHARED
                    declarativepersonsmodel.cpp
                    declarativepersondata.cpp
                    peopleqmlplugin.cpp)

target_link_libraries(kpeopledeclarative ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${KDE4_KDECORE_LIBRARY} kpeople)

install(TARGETS kpeopledeclarative DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/people)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/people)
