# Find libkpeople
# Once done this will define
#
#  KPEOPLE_FOUND    - system has KPeople Library
#  KPEOPLE_INCLUDES - the KPeople include directory
#  KPEOPLE_LIBS     - link these to use KPeople
#  KPEOPLE_VERSION  - the version of the KPeople Library

# Copyright (c) 2012, Aleix Pol Gonzalez <aleixpol@blue-systems.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_library(KPEOPLE_LIBRARY NAMES kpeople HINTS ${KDE4_LIB_INSTALL_DIR} ${QT_LIBRARY_DIR})
find_library(KPEOPLEWIDGETS_LIBRARY NAMES kpeoplewidgets HINTS ${KDE4_LIB_INSTALL_DIR} ${QT_LIBRARY_DIR})

find_path(KPEOPLE_INCLUDE_DIR NAMES kpeople_export.h HINTS ${KDE4_INCLUDE_INSTALL_DIR} ${QT_INCLUDE_DIR} ${INCLUDE_INSTALL_DIR} PATH_SUFFIXES kpeople)
get_filename_component(KPEOPLE_INCLUDE_DIR ${KPEOPLE_INCLUDE_DIR} PATH)

if(KPEOPLE_INCLUDE_DIR AND KPEOPLE_LIBRARY)
   set(KPEOPLE_LIBS ${kpeople_LIB_DEPENDS} ${KPEOPLE_LIBRARY})
   set(KPEOPLE_INCLUDES ${KPEOPLE_INCLUDE_DIR})
endif(KPEOPLE_INCLUDE_DIR AND KPEOPLE_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KPeople  DEFAULT_MSG KPEOPLE_INCLUDE_DIR KPEOPLE_LIBRARY)

mark_as_advanced(KPEOPLE_INCLUDE_DIR KPEOPLE_LIBRARY)
