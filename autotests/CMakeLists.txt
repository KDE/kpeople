find_package(Qt5Test)

ecm_add_test(TEST_NAME persondatatest persondatatests.cpp fakecontactsource.cpp LINK_LIBRARIES Qt5::Sql Qt5::Test KF5::People KF5::PeopleBackend)
ecm_add_test(TEST_NAME personsmodeltest personsmodeltest.cpp fakecontactsource.cpp LINK_LIBRARIES Qt5::Sql Qt5::Test KF5::People KF5::PeopleBackend)
