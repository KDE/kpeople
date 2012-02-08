project (kpeople)

set (CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
     ${CMAKE_MODULE_PATH}
)

set (KDE_MIN_VERSION "4.6.60")
set (SDO_MIN_VERSION "0.7.0")

set(IS_KTP_INTERNAL_MODULE TRUE)

find_package (KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package (SharedDesktopOntologies ${SDO_MIN_VERSION} REQUIRED)
find_package (Nepomuk REQUIRED)
find_package (Soprano REQUIRED)
find_package (TelepathyQt4 0.8.90 REQUIRED)
find_package (KTp REQUIRED)

include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${KDE4_DEFINITIONS}
                 -DDISABLE_NEPOMUK_LEGACY
                 -DQT_NO_CAST_FROM_ASCII
)

include_directories (${KDE4_INCLUDES}
                     ${NEPOMUK_INCLUDES}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${TELEPATHY_QT4_INCLUDE_DIR}
                     ${KTP_INCLUDE_DIR}
)

# add_subdirectory (ontologies)
add_subdirectory (src)