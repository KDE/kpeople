cmake_minimum_required(VERSION 3.0)

set(KF5_VERSION "5.47.0") # handled by release scripts
set(KF5_DEP_VERSION "5.47.0") # handled by release scripts
project (KPeople VERSION ${KF5_VERSION})

include(FeatureSummary)
find_package(ECM 5.47.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(REQUIRED_QT_VERSION 5.8.0)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Gui Sql DBus Widgets Qml)

find_package(KF5CoreAddons ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5WidgetsAddons ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5ItemViews ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5Service ${KF5_DEP_VERSION} CONFIG REQUIRED)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMSetupVersion)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMAddQch)
include(CMakePackageConfigHelpers)
include(ECMGeneratePriFile)
include(GenerateExportHeader)

include(KDEInstallDirs)
include(KDECMakeSettings)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

add_definitions(-DTRANSLATION_DOMAIN=\"kpeople5\")

set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5People")

ecm_setup_version(PROJECT VARIABLE_PREFIX KPEOPLE
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kpeople_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfigVersion.cmake"
                        SOVERSION 5)

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

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5People_QCH
        FILE KF5PeopleQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5PeopleQchTargets.cmake\")")
endif()

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5PeopleConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(EXPORT KPeopleTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPeopleTargets.cmake NAMESPACE KF5::)
install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
