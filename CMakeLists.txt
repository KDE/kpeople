project (kpeople)

set (KDE_MIN_VERSION "4.6.60")
set (SDO_MIN_VERSION "0.7.0")

find_package (KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package (SharedDesktopOntologies ${SDO_MIN_VERSION} REQUIRED)
find_package (Nepomuk REQUIRED)
find_package (Soprano REQUIRED)

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
)

add_subdirectory (ontologies)
add_subdirectory (src)

