project (kpeople)

option(ENABLE_EXAMPLES "Enable building of examples" ON)
option(KDE4_BUILD_TESTS "Enable building of tests" ON)

set (CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
     ${CMAKE_MODULE_PATH}
)

set (KDE_MIN_VERSION "4.10.1")
set (SDO_MIN_VERSION "0.7.0")

set(KPEOPLE_LIB_VERSION 0.2)
set(KPEOPLE_LIB_SOVERSION 3)

find_package (KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package (KdepimLibs REQUIRED)

include (KDE4Defaults)
include (MacroLibrary)

include_directories (${KDE4_INCLUDES}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${KDEPIMLIBS_INCLUDE_DIRS}
)

add_subdirectory (src)
add_subdirectory (pixmaps)

set(module_install_dir ${DATA_INSTALL_DIR}/cmake/modules )
install(FILES cmake/FindKPeople.cmake DESTINATION ${module_install_dir})
