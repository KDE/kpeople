include_directories(..)

# Email plugin
# add_library(emaildetailswidgetplugin MODULE emaildetailswidget.cpp)
# target_link_libraries(emaildetailswidgetplugin Qt5::Core Qt5::Gui ${KDE4_KDECORE_LIBRARY} KF5::People KF5::PeopleWidgets)
#
# install(TARGETS emaildetailswidgetplugin DESTINATION ${KDE_INSTALL_PLUGINDIR})
# install(FILES emaildetailswidgetplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
#
# #  Merge Plugin
# add_library(mergecontactswidgetplugin MODULE mergecontactswidget.cpp personpresentationwidget.cpp)
# target_link_libraries(mergecontactswidgetplugin Qt5::Core Qt5::Gui ${KDE4_KDECORE_LIBRARY} KF5::People KF5::PeopleWidgets)
#
# install(TARGETS mergecontactswidgetplugin DESTINATION ${KDE_INSTALL_PLUGINDIR})
# install(FILES mergecontactswidgetplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
#
# # Phone plugin
# add_library(phonedetailswidgetplugin MODULE phonedetailswidget.cpp)
# target_link_libraries(phonedetailswidgetplugin Qt5::Core Qt5::Gui ${KDE4_KDECORE_LIBRARY} KF5::People KF5::PeopleWidgets)
#
# install(TARGETS phonedetailswidgetplugin DESTINATION ${KDE_INSTALL_PLUGINDIR})
# install(FILES phonedetailswidgetplugin.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
