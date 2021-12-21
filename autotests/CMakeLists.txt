find_package(Qt${QT_MAJOR_VERSION}Test)

set(libs Qt${QT_MAJOR_VERSION}::Sql Qt${QT_MAJOR_VERSION}::Test KF5::People KF5::PeopleBackend)

ecm_add_test(TEST_NAME persondatatest persondatatests.cpp fakecontactsource.cpp LINK_LIBRARIES ${libs})
ecm_add_test(TEST_NAME personsmodeltest personsmodeltest.cpp fakecontactsource.cpp LINK_LIBRARIES ${libs})
ecm_add_test(TEST_NAME personsproxymodeltest personsproxymodeltest.cpp fakecontactsource.cpp LINK_LIBRARIES ${libs})
