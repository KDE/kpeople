kde4_add_unit_test(persondatatest persondatatests.cpp fakecontactsource.cpp)
target_link_libraries(persondatatest
    ${QT_QTCORE_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    ${KDEPIMLIBS_KABC_LIBS}
    kpeople)

# add_test(PersonDataTests persondatatest)
