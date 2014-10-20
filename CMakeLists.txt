cmake_minimum_required(VERSION 2.8.12)

project (KPeople)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 5.3 REQUIRED Gui Sql DBus Widgets)
find_package(KF5 5.3 REQUIRED CoreAddons KIO I18n)
find_package(KF5Abc REQUIRED)
find_package(KF5Baloo)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMPackageConfigHelpers)
include(GenerateExportHeader)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

add_definitions(-DTRANSLATION_DOMAIN=\"libkpeople\")

set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5People")

ecm_setup_version(0.5.0 VARIABLE_PREFIX KPEOPLE
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kpeople_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfigVersion.cmake"
                        SOVERSION 5)

ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5PeopleConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)


add_subdirectory (src)
add_subdirectory (pixmaps)

install(EXPORT KPeopleTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPeopleTargets.cmake NAMESPACE KF5::)
install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )


feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
