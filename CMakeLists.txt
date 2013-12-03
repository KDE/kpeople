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

#find_package (SharedDesktopOntologies ${SDO_MIN_VERSION} REQUIRED) #TODO Remove
#find_package (NepomukCore REQUIRED) #TODO Remove
#find_package (Soprano REQUIRED) #TODO Remove

include (KDE4Defaults)
include (MacroLibrary)

include_directories (${KDE4_INCLUDES}
#                     ${NEPOMUK_CORE_INCLUDE_DIR} #TODO, Remove
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory (src)
add_subdirectory (pixmaps)

set(module_install_dir ${DATA_INSTALL_DIR}/cmake/modules )
install(FILES cmake/FindKPeople.cmake DESTINATION ${module_install_dir})
