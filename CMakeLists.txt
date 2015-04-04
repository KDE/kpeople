cmake_minimum_required(VERSION 2.8.12)

project (KPeople)

find_package(ECM 1.8.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(KF5_VERSION "5.9.0") # handled by release scripts
set(KF5_DEP_VERSION "5.8.0") # handled by release scripts

set(REQUIRED_QT_VERSION 5.2.0)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Gui Sql DBus Widgets Qml)

find_package(KF5CoreAddons ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5Service ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5WidgetsAddons ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5ItemViews ${KF5_DEP_VERSION} CONFIG REQUIRED)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMPackageConfigHelpers)
include(ECMGeneratePriFile)
include(GenerateExportHeader)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

add_definitions(-DTRANSLATION_DOMAIN=\"libkpeople\")

set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5People")

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX KPEOPLE
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kpeople_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfigVersion.cmake"
                        SOVERSION 5)

ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5PeopleConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

option(ENABLE_EXAMPLES "Build the examples" "ON")

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif ()
add_subdirectory (src)
add_subdirectory (pixmaps)
if (ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()
add_subdirectory(autotests)

install(EXPORT KPeopleTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPeopleTargets.cmake NAMESPACE KF5::)
install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
