AC_INIT([libsllp], [1.00], [bruno.martins@lnls.br])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign -Wall -Werror silent-rules])
# Makes ./configure --enable-silent-rules possible

AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_CC_C_O

AC_CHECK_HEADERS([stdlib.h stdbool.h stdint.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
