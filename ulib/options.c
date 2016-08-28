#include "options.h"
#include <string.h>

/* Parse the command line according to option descriptions in
   OPTS.  */
int
ulib_options_parse (const ulib_option *opts, int argc, const char *argv[],
                    FILE *err)
{
  int next_arg, next_arg_taken, key;
  const char *opt_name, *opt_arg;
  unsigned int len;
  const ulib_option *opt;

  key = -1;
  next_arg = 1;
  next_arg_taken = 0;
  while (next_arg < argc)
    {
      opt_arg = 0;
      opt_name = 0;
      len = 0;

      /* Get next option.  */
      if (key == -1)
        {
          /* Figure out if we have a short or a long option.  */
          if (argv [next_arg][0] == '-')
            {
              if (argv [next_arg][1] == '-')
                {
                  if (argv [next_arg][2] == '\0')
                    {
                      /* End of options and start of non-option
                         arguments.  */
                      ++next_arg;
                      break;
                    }

                  /* Separate the option name from a possible argument.  */
                  opt_name = argv [next_arg] + 2;
                  opt_arg = strchr (opt_name, '=');
                  if (opt_arg == 0)
                    len = strlen (opt_name);
                  else
                    len = opt_arg - opt_name;
                }
              else
                key = 1;
            }
        }

      /* Search the option descriptions.  */
      for (opt = opts; opt->key != 0 || opt->name != 0; ++opt)
        {
          if ((key != -1 && opt->key == argv [next_arg][key])
              || (key == -1
                  && opt->name != 0 && opt_name != 0
                  && strncmp (opt->name, opt_name, len) == 0))
            break;
        }

      if (opt->key == 0 && opt->name == 0)
        {
          /* Unrecognized option or a non-option argument.  */
          if (argv [next_arg][0] == '-')
            {
              if (key == -1)
                fprintf (err, "%s: ERROR: Unknown option ``%s''\n",
                         argv [0], argv [next_arg]);
              else
                fprintf (err, "%s: ERROR: Unknown option ``%c''\n",
                         argv [0], argv [next_arg][key]);
              return -1;
            }
          else if (opt->cb == 0)
            {
              fprintf (err, "%s: WARNING: Non-option argument ``%s''"
                       " not processed\n", argv [0], argv [next_arg]);
            }
          else if (opt->cb (argv [next_arg]) < 0)
            return -1;
        }
      else
        {
          /* Skip the ``='' */
          if (opt_arg)
            ++opt_arg;

          /* Check for missing or extra arguments.  */
          if (opt->flags == ulib_option_no_arg)
            {
              if (opt_arg != 0)
                {
                  if (key == -1)
                    fprintf (err, "%s: ERROR: Option ``--%s'' takes no"
                             " arguments\n", argv [0], opt->name);
                  return -1;
                }
            }
          else if (opt->flags == ulib_option_required_arg)
            {
              if (opt_arg == 0)
                {
                  /* If the next argument does not begin with a dash,
                     consider it an argument for the current
                     option.  */
                  if (! next_arg_taken
                      && next_arg + 1 < argc
                      && argv [next_arg + 1][0] != '-')
                    {
                      opt_arg = argv [next_arg + 1];
                      next_arg_taken = 1;
                    }
                  else
                    {
                      if (key == -1)
                        fprintf (err, "%s: ERROR: Option ``--%s'' requires"
                                 " an argument\n", argv [0], opt->name);
                      else
                        fprintf (err, "%s: ERROR: Option ``-%c'' requires"
                                 " an argument\n", argv [0], opt->key);

                      return -1;
                    }
                }
            }
          else /* opt->flags == ulib_option_optional_arg */
            {
              if (opt_arg == 0)
                {
                  /* If the next argument does not begin with a dash,
                     consider it an argument for the current
                     option.  */
                  if (! next_arg_taken
                      && next_arg + 1 < argc
                      && argv [next_arg + 1][0] != '-')
                    {
                      opt_arg = argv [next_arg + 1];
                      next_arg_taken = 1;
                    }
                }
            }

          /* Set the FLAG value.  */
          if (opt->flag != 0)
            *opt->flag = opt->value;

          /* Invoke the callback.  */
          if (opt->cb == 0 && opt_arg != 0)
            {
              fprintf (err, "%s: ERROR: Unable to process option"
                       " argument ``%s''\n", argv [0], opt_arg);
              return -1;
            }

          if (opt->cb && opt->cb (opt_arg) < 0)
            return -1;
        }

      if (key != -1)
        {
          ++key;
          if (argv [next_arg][key] == '\0')
            {
              key = -1;
              next_arg += next_arg_taken ? 2 : 1;
              next_arg_taken = 0;
            }
        }
      else
        {
          next_arg += next_arg_taken ? 2 : 1;
          next_arg_taken = 0;
        }
    }

  /* Get the non-option arguments handler.  */
  for (opt = opts; opt->key != 0 || opt->name != 0; ++opt)
    ;

  /* Process remaining non-option arguments.  */
  while (next_arg < argc)
    {
      if (opt->cb == 0)
        {
          fprintf (err, "%s: WARNING: Non-option argument ``%s''"
                   " not processed\n", argv [0], argv [next_arg]);
        }
      else if (opt->cb (argv [next_arg]) < 0)
        return -1;

      ++next_arg;
    }

  return 0;
}

/* Display options help.  */
void
ulib_options_help (FILE *out, const ulib_option *opts)
{
  const ulib_option *opt;
  const char *arg;

  for (opt = opts; opt->key || opt->name; ++opt)
    {
      if (opt->arg)
        arg = opt->arg;
      else
        arg = "<arg>";

      if (opt->key && opt->name)
        {
          if (opt->flags == ulib_option_required_arg)
            fprintf (out, "  -%c %s, --%s=%s", opt->key, arg, opt->name, arg);
          else if (opt->flags == ulib_option_optional_arg)
            fprintf (out, "  -%c [%s], --%s[=%s]", opt->key, arg, opt->name,
                     arg);
          else
            fprintf (out, "  -%c, --%s", opt->key, opt->name);
        }
      else
        {
          if (opt->key)
            {
              fprintf (out, "  -%c", opt->key);
              if (opt->flags == ulib_option_required_arg)
                fprintf (out, " %s", arg);
              else if (opt->flags == ulib_option_optional_arg)
                fprintf (out, " [%s]", arg);
            }
          else if (opt->name)
            {
              fprintf (out, "-  -%s", opt->name);
              if (opt->flags == ulib_option_required_arg)
                fprintf (out, "=%s", arg);
              else if (opt->flags == ulib_option_optional_arg)
                fprintf (out, "[=%s]", arg);
            }
        }

      if (opt->help)
        fprintf (out, " %s\n", opt->help);
      else
        fputs (" No help available for option\n", out);
    }
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
