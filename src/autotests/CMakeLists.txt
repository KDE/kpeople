set (NEPOMUK_TESTLIB_TOOLS_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/lib/tools")

set (NEPOMUK_TESTLIB_CMAKE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/lib")

set (NEPOMUK_TESTLIB_DATA_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/lib")

add_subdirectory( lib )
add_subdirectory( tests )
