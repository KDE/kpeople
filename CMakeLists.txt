cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.103.0") # handled by release scripts
set(KF_DEP_VERSION "5.103.0") # handled by release scripts
project (KPeople VERSION ${KF_VERSION})

include(FeatureSummary)
find_package(ECM 5.103.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(KDEGitCommitHooks)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMAddQch)
include(CMakePackageConfigHelpers)
include(ECMGeneratePriFile)
include(GenerateExportHeader)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)

set(REQUIRED_QT_VERSION 5.15.2)
find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Gui Sql Widgets Qml Quick)

# On non-Apple UNIX DBus integration is required, on Android it does not exist and on other platforms it is optional
if(UNIX AND NOT APPLE)
    if(NOT ANDROID)
        find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED DBus)
    endif()
else()
    find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} OPTIONAL_COMPONENTS DBus)
endif()

find_package(KF5CoreAddons ${KF_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5WidgetsAddons ${KF_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF_DEP_VERSION} CONFIG REQUIRED)
find_package(KF5ItemViews ${KF_DEP_VERSION} CONFIG REQUIRED)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

add_definitions(-DTRANSLATION_DOMAIN=\"kpeople5\")

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5People")

ecm_setup_version(PROJECT VARIABLE_PREFIX KPEOPLE
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kpeople_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5PeopleConfigVersion.cmake"
                        SOVERSION 5)

option(ENABLE_EXAMPLES "Build the examples" "ON")

ki18n_install(po)

ecm_set_disabled_deprecation_versions(
    QT 5.15.2
    KF 5.95
)

add_subdirectory (src)
if (ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

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

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
