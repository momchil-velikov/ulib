# generated automatically by aclocal 1.7.9 -*- Autoconf -*-

# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

dnl
dnl Standard checks
dnl
AC_DEFUN(AC_STANDARD_CHECKS, [
AC_CANONICAL_SYSTEM
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_EGREP
AC_CHECK_TOOL(AR,ar,false)
AC_CHECK_TOOL(RANLIB,ranlib,false)

AC_ENABLE_DEBUG
AC_ENABLE_OPTIMIZE
AC_ENABLE_OPTIMIZE_SPACE
AC_ENABLE_WARNINGS
AC_HOST_CC
AC_ENABLE_HOST_CPU
])
# arch-tag: 6027d1b0-606f-4ab1-b419-e310302a536a

dnl
dnl Whether to include debugging information
dnl
AC_DEFUN(AC_ENABLE_DEBUG,[
changequote(<<,>>)
CFLAGS=$(echo $CFLAGS | sed -e s/-g//g)
CXXFLAGS=$(echo $CXXFLAGS | sed -e s/-g//g)
changequote([,])
AC_ARG_ENABLE(debug,
[--enable-debug			Include debug info (default=yes)],
[ if test x"${enable_debug}" = xyes
  then
    enable_debug=1; enable_checks=1;
  else
    enable_debug=0; enable_checks=0
  fi
],
[ enable_debug=1; enable_checks=1 ])
AC_DEFINE_UNQUOTED(ENABLE_DEBUG, ${enable_debug},
                   [Generate debugging informaion. ])
AC_SUBST(enable_debug)
AC_SUBST(enable_checks)])

# arch-tag: 6e990e3d-0142-4bf3-8ba7-e732d3af1601

dnl
dnl Select optimized/non-optimized build
dnl
AC_DEFUN(AC_ENABLE_OPTIMIZE, [
changequote(<<,>>)
CFLAGS=$(echo $CFLAGS | sed -e s/-O[0-9]*//g)
CXXFLAGS=$(echo $CXXFLAGS | sed -e s/-O[0-9]*//g)
changequote([,])
AC_ARG_ENABLE(optimize,
[--enable-optimize		Build with optimization (default=no)],
[ if test x"${enable_optimize}" = xyes
  then
    enable_optimize=1
  else
    enable_optimize=0
  fi
],
[ enable_optimize=0 ])
AC_DEFINE_UNQUOTED(ENABLE_OPTIMIZE, ${enable_optimize},
                   [Build with optimizations. ])
AC_SUBST(enable_optimize)])

# arch-tag: 8713311d-34dc-45fa-963e-db1912db8d80

dnl
dnl Favor space over time.
dnl
AC_DEFUN(AC_ENABLE_OPTIMIZE_SPACE, [
AC_ARG_ENABLE(optimize,
[--enable-optimize-space		Favor smaller code and data size (default=no)],
[ if test x"${enable_optimize_space}" = xyes
  then
    enable_optimize_space=1
  else
    enable_optimize_space=0
  fi
],
[ enable_optimize_space=0 ])
AC_DEFINE_UNQUOTED(ENABLE_OPTIMIZE_SPACE, ${enable_optimize_space},
                   [Favor smaller code and data size. ])
AC_SUBST(enable_optimize_space)])

# arch-tag: f497d48b-21f5-4361-b6e8-7ef949b9b2a6


dnl
dnl Extra warnings
dnl
AC_DEFUN(AC_ENABLE_WARNINGS, [
AC_ARG_ENABLE(warnings,
[--enable-warnings		Generate extra warnings (default=yes)],
[ if test x"${enable_warnings}" = xyes
  then
    enable_warnings=1
  else
    enable_warnings=0
  fi
],
[ enable_warnings=1 ])
AC_DEFINE_UNQUOTED(ENABLE_WARNINGS, ${enable_warnings},
                   [Generate extra warnings. ])
AC_SUBST(enable_warnings)])

# arch-tag: eeffc1ba-2ded-46d7-9ecf-7a12d1ef1971

dnl
dnl Recognize host C compiler
dnl
AC_DEFUN(AC_HOST_CC, [
host_cc=cc
dnl Check for GCC.
if test "$GCC" = "yes"; then
  host_cc=gcc
else
dnl Check for Intel CC.
  AC_MSG_CHECKING(for Intel C/C++ compiler)
  AC_EGREP_CPP([icc], [#if defined __INTEL_COMPILER
                         icc
                       #endif],
                AC_MSG_RESULT(yes)
                host_cc=icc,
                AC_MSG_RESULT(no))
fi
host_compiler_frag="${srcdir}/config/mk/${host_cc}.mk"
AC_SUBST_FILE(host_compiler_frag)])

# arch-tag: 76981131-c8c0-4144-822c-887b228473e4


dnl
dnl Select host CPU.
dnl
AC_DEFUN(AC_ENABLE_HOST_CPU, [
host_cpu_opt=
AC_ARG_ENABLE(host_cpu,
[--enable-host-cpu=CPU		Build with optimizations for CPU],
[ case x"${enable_host_cpu}" in
  xyes|xno)
    AC_MSG_WARN([--enable-host-cpu takes a CPU name argument])
    ;;
  *)
    host_cpu_opt="${enable_host_cpu}"
    ;;
  esac
])
AC_SUBST(host_cpu_opt)

changequote(<<,>>)
case "${host_cpu}" in
i[34567]86)
  host_arch_compiler_frag="${srcdir}/config/mk/ia32-${host_cc}.mk"
  ;;
powerpc*|rs6000)
  host_arch_compiler_frag="${srcdir}/config/mk/powerpc-${host_cc}.mk"
  ;;
arm*)
  host_arch_compiler_frag="${srcdir}/config/mk/arm-${host_cc}.mk"
  ;;
*)
  host_arch_compiler_frag="${srcdir}/config/mk/${host_cpu}-${host_cc}.mk"
  ;;
esac

if test ! -f "${host_arch_compiler_frag}"; then
  host_arch_compiler_frag="${srcdir}/config/mk/empty.mk"
fi
changequote([,])

AC_SUBST_FILE(host_arch_compiler_frag)])

# arch-tag: 36340bc6-59eb-4278-9e2c-b285b07ee844

