# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(ulib, 0.8)
AC_CONFIG_SRCDIR(ulib/alloc.c)
AC_CONFIG_HEADER(ulib/config.h)

# Standard checks
AC_STANDARD_CHECKS

# Checks for programs.

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stddef.h stdint.h stdlib.h string.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_TYPES(uintptr_t)

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset])

# Autoheader stuff.  
AH_TOP([
#ifndef ulib__config_h
#define ulib__config_h])

AH_BOTTOM(
[#endif /* ulib__config_h */])

AC_CONFIG_FILES([Makefile config.mk:config/config.mk.in ulib/Makefile
                 test/Makefile])
AC_OUTPUT
