#ifndef KPEOPLE_EXPORT_H
#define KPEOPLE_EXPORT_H

// needed for KDE_EXPORT and KDE_IMPORT macros
#include <kdemacros.h>

#ifndef KPEOPLE_EXPORT
# if defined(MAKE_KPEOPLE_LIB)
// We are building this library
#  define KPEOPLE_EXPORT KDE_EXPORT
# else
// We are using this library
#  define KPEOPLE_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KPEOPLE_EXPORT_DEPRECATED
#  define KPEOPLE_EXPORT_DEPRECATED KDE_DEPRECATED KPEOPLE_EXPORT
# endif

#endif
