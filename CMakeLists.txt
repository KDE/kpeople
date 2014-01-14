project (kpeople)

option(ENABLE_EXAMPLES "Enable building of examples" ON)
option(KDE4_BUILD_TESTS "Enable building of tests" ON)

set (CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
     ${CMAKE_MODULE_PATH}
)

set (KDE_MIN_VERSION "4.10.1")
set (SDO_MIN_VERSION "0.7.0")

set(KPEOPLE_VERSION_MAJOR 0)
set(KPEOPLE_VERSION_MINOR 2)
set(KPEOPLE_VERSION_PATCH 0)

set(KPEOPLE_VERSION "${KPEOPLE_VERSION_MAJOR}.${KPEOPLE_VERSION_MINOR}.${KPEOPLE_VERSION_PATCH}")
set(KPEOPLE_SOVERSION 3)

find_package (KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package (KdepimLibs REQUIRED)

include (KDE4Defaults)
include (MacroLibrary)

include_directories (${KDE4_INCLUDES}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${KDEPIMLIBS_INCLUDE_DIRS}
)

if(KdepimLibs_VERSION_MINOR GREATER 11)
    add_definitions(-DHAVE_KDEPIM_ATLEAST_412)
endif()

add_subdirectory (src)
add_subdirectory (pixmaps)

configure_file(cmake/KPeopleConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/KPeopleConfig.cmake @ONLY)

macro_write_basic_cmake_version_file(${CMAKE_CURRENT_BINARY_DIR}/KPeopleConfigVersion.cmake
        ${KPEOPLE_VERSION_MAJOR} ${KPEOPLE_VERSION_MINOR} ${KPEOPLE_VERSION_PATCH})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/KPeopleConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/cmake/KPeopleConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/cmake/KPeople)
