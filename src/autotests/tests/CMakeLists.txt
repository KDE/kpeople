include(${NEPOMUK_TESTLIB_CMAKE_DIR}/NepomukTestLibMacros.cmake)

#
# Person Data Tests
#

set( PERSON_DATA_TESTS_SRC persondatatests.cpp )

kde4_add_executable(persondatatest ${PERSON_DATA_TESTS_SRC})

target_link_libraries(persondatatest
  ${QT_QTCORE_LIBRARY}
  ${QT_QTTEST_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDECORE_LIBS}
  ${NEPOMUK_CORE_LIBRARY}
  nepomuktestlib
  kpeople
)
add_nepomuk_test(PersonDataTest ${CMAKE_CURRENT_BINARY_DIR}/persondatatest)


#
# Duplicates Tests
#

set( DUPLICATES_TESTS_SRC duplicatestests.cpp )

kde4_add_executable(duplicatestest ${DUPLICATES_TESTS_SRC})

target_link_libraries(duplicatestest
  ${QT_QTCORE_LIBRARY}
  ${QT_QTTEST_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDECORE_LIBS}
  ${NEPOMUK_CORE_LIBRARY}
  nepomuktestlib
  kpeople
)
add_nepomuk_test(DuplicatesTest ${CMAKE_CURRENT_BINARY_DIR}/duplicatestest)

#
# Model Tests
#

set( MODEL_TESTS_SRC modeltests.cpp )

kde4_add_executable(modeltest ${MODEL_TESTS_SRC})

target_link_libraries(modeltest
  ${QT_QTCORE_LIBRARY}
  ${QT_QTTEST_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDECORE_LIBS}
  ${NEPOMUK_CORE_LIBRARY}
  nepomuktestlib
  kpeople
)
add_nepomuk_test(ModelTest ${CMAKE_CURRENT_BINARY_DIR}/modeltest)
