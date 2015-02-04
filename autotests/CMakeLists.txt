find_package(Qt5Test)

ecm_add_test(TEST_NAME persondatatest persondatatests.cpp fakecontactsource.cpp)
target_link_libraries(persondatatest
    Qt5::Sql
    Qt5::Test
    KPeople
    KPeopleBackends
)
