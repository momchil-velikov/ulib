#ifndef ulib__options_h
#define ulib__options_h 1

#include "defs.h"
#include "ulib-if.h"
#include <stdio.h>

BEGIN_DECLS

/* Callback function type. If the option argument is not present or
   the option requires no argumewnt, the OPTARG parameter is NULL.  */
typedef int (*ulib_opt_cb) (const char *optarg);

/* Option flags.  */
enum ulib_option_flags
{
  ulib_option_no_arg,
  ulib_option_required_arg,
  ulib_option_optional_arg
};

/* Option desctiption.  */
struct ulib_option
{
  /* Short option name.  */
  int key;

  /* Long option name.  */
  const char *name;

  /* Location, into which to store the value of the VALUE field,
     whenever the option is encountered.  */
  int *flag;
  
  /* Value to set the FLAG member to.  */
  int value;

  /* Option callback.  */
  ulib_opt_cb cb;

  /* Option flags.  */
  unsigned int flags;

  /* Option arg string.  */
  const char *arg;

  /* Option help string.  */
  const char *help;
};
typedef struct ulib_option ulib_option;

/* Parse the command line according to option descriptions in
   OPTS.  */
ULIB_IF int ulib_options_parse (const ulib_option *opts, int argc,
                                const char *argv[], FILE *err);

/* Display options help.  */
ULIB_IF void ulib_options_help (FILE *out, const ulib_option *opts);

END_DECLS
#endif /* ulib__options_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
