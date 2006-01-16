#include <ulib/options.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef strcmp
#undef strcmp
#endif

static int
process_non_option_arg (const char *arg)
{
  assert (arg != 0);
  fprintf (stderr, "test: INFO: Found non-option argument ``%s''\n", arg);
  return 0;
}

static int d_flag;
static int i_flag;
static int m_flag;

static int e_cb_flag;
static int
e_cb (const char *arg __attribute__ ((unused)))
{
  e_cb_flag = 2;
  return 0;
}

static const char *f_cb_arg;
static int
f_cb (const char *arg)
{
  f_cb_arg = arg;
  return 0;
}

static const char *g_cb_arg;
static int
g_cb (const char *arg)
{
  g_cb_arg = arg;
  return 0;
}

static const char *h_cb_arg;
static int
h_cb (const char *arg)
{
  h_cb_arg = arg;
  return 0;
}

static const char *i_cb_arg;
static int
i_cb (const char *arg)
{
  i_cb_arg = arg;
  return 0;
}

static const char *j_cb_arg;
static int
j_cb (const char *arg)
{
  j_cb_arg = arg;
  return 0;
}

static const char *k_cb_arg;
static int
k_cb (const char *arg)
{
  k_cb_arg = arg;
  return 0;
}

static const char *l_cb_arg;
static int
l_cb (const char *arg)
{
  l_cb_arg = arg;
  return 0;
}

static const char *m_cb_arg;
static int
m_cb (const char *arg)
{
  m_cb_arg = arg;
  return 0;
}

static struct ulib_option options [] =
{
  {.key = 'a' },

  { .name = "b-long" },

  { .key = 'c', .name = "c-long" },

  { .key = 'd', .name = "d-long", .flag = &d_flag, .value = 1 },

  { .key = 'e', .name = "e-long", .cb = e_cb },


  {
    .key = 'f',
    .cb = f_cb,
    .flags = ulib_option_optional_arg
  },

  {
    .name = "g-long",
    .cb = g_cb,
    .flags = ulib_option_optional_arg
  },

  {
    .key = 'h',
    .name = "h-long",
    .cb = h_cb,
    .flags = ulib_option_optional_arg
  },

  {
    .key = 'i',
    .name = "i-long",
    .flag = &i_flag,
    .value = 3,
    .cb = i_cb,
    .flags = ulib_option_optional_arg
  },


  {
    .key = 'j',
    .cb = j_cb,
    .flags = ulib_option_required_arg
  },

  {
    .name = "k-long",
    .cb = k_cb,
    .flags = ulib_option_required_arg
  },

  {
    .key = 'l',
    .name = "l-long",
    .cb = l_cb,
    .flags = ulib_option_required_arg
  },

  {
    .key = 'm',
    .name = "m-long",
    .flag = &m_flag,
    .value = 4,
    .cb = m_cb,
    .flags = ulib_option_required_arg
  },


  {
    .key = '\0', .name = 0,
    .flag = 0, .value = 0,
    .cb = process_non_option_arg, .flags = 0,
    .help = 0
  }
};


static void
test_a ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-a", 0};

  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);
}

static void
test_b ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "--b-long", 0};
  static const char *argv_1 [] = {"test", "--b-long=", 0};
  static const char *argv_2 [] = {"test", "--b-long=foo", 0};

  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);

  sts = ulib_options_parse (options, 2, argv_1, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options, 2, argv_2, stderr);
  assert (sts == -1);
}

static void
test_c ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-c", 0};
  static const char *argv_1 [] = {"test", "--c-long", 0};
  static const char *argv_2 [] = {"test", "--c-long=", 0};
  static const char *argv_3 [] = {"test", "--c-long=foo", 0};

  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);

  sts = ulib_options_parse (options, 2, argv_1, stderr);
  assert (sts == 0);

  sts = ulib_options_parse (options, 2, argv_2, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options, 2, argv_3, stderr);
  assert (sts == -1);
}

static void
test_d ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-d", 0};
  static const char *argv_1 [] = {"test", "--d-long", 0};
  static const char *argv_2 [] = {"test", "--d-long=", 0};
  static const char *argv_3 [] = {"test", "--d-long=foo", 0};

  d_flag = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);
  assert (d_flag == 1);

  d_flag = 0;
  sts = ulib_options_parse (options, 2, argv_1, stderr);
  assert (sts == 0);
  assert (d_flag == 1);

  d_flag = 0;
  sts = ulib_options_parse (options, 2, argv_2, stderr);
  assert (sts == -1);
  assert (d_flag == 0);

  d_flag = 0;
  sts = ulib_options_parse (options, 2, argv_3, stderr);
  assert (sts == -1);
  assert (d_flag == 0);
}

static void
test_e ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-e", 0};
  static const char *argv_1 [] = {"test", "--e-long", 0};
  static const char *argv_2 [] = {"test", "--e-long=", 0};
  static const char *argv_3 [] = {"test", "--e-long=foo", 0};

  e_cb_flag = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);
  assert (e_cb_flag == 2);

  e_cb_flag = 0;
  sts = ulib_options_parse (options, 2, argv_1, stderr);
  assert (sts == 0);
  assert (e_cb_flag == 2);

  e_cb_flag = 0;
  sts = ulib_options_parse (options, 2, argv_2, stderr);
  assert (sts == -1);
  assert (e_cb_flag == 0);

  e_cb_flag = 0;
  sts = ulib_options_parse (options, 2, argv_3, stderr);
  assert (sts == -1);
  assert (e_cb_flag == 0);
}


static void
test_f ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-f", 0};
  static const char *argv_1 [] = {"test", "-f", "f-arg", 0};
  static const char *argv_2 [] = {"test", "-f", "-a", 0};

  f_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);
  assert (f_cb_arg == 0);

  f_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0);
  assert (f_cb_arg != 0 && strcmp (f_cb_arg, "f-arg") == 0);

  f_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == 0);
  assert (f_cb_arg == 0);
}

static void
test_g ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "--g-long", 0};
  static const char *argv_1 [] = {"test", "--g-long", "g-arg", 0};
  static const char *argv_2 [] = {"test", "--g-long=g-arg-1", "g-arg", 0};
  static const char *argv_3 [] = {"test", "--g-long", "-a", 0};

  g_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);
  assert (g_cb_arg == 0);

  g_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0);
  assert (g_cb_arg != 0 && strcmp (g_cb_arg, "g-arg") == 0);

  g_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == 0);
  assert (g_cb_arg != 0 && strcmp (g_cb_arg, "g-arg-1") == 0);

  g_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_3, stderr);
  assert (sts == 0);
  assert (g_cb_arg == 0);
}

static void
test_h ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "--h-long", 0};
  static const char *argv_1 [] = {"test", "--h-long", "h-arg", 0};
  static const char *argv_2 [] = {"test", "--h-long=h-arg-1", "h-arg", 0};
  static const char *argv_3 [] = {"test", "--h-long", "-a", 0};

  static const char *argv_4 [] = {"test", "-h", 0};
  static const char *argv_5 [] = {"test", "-h", "h-arg", 0};
  static const char *argv_6 [] = {"test", "-h", "-a", 0};

  h_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);
  assert (h_cb_arg == 0);

  h_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0);
  assert (h_cb_arg != 0 && strcmp (h_cb_arg, "h-arg") == 0);

  h_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == 0);
  assert (h_cb_arg != 0 && strcmp (h_cb_arg, "h-arg-1") == 0);

  h_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_3, stderr);
  assert (sts == 0);
  assert (h_cb_arg == 0);

  h_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_4, stderr);
  assert (sts == 0);
  assert (h_cb_arg == 0);

  h_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_5, stderr);
  assert (sts == 0);
  assert (h_cb_arg != 0 && strcmp (h_cb_arg, "h-arg") == 0);

  h_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_6, stderr);
  assert (sts == 0);
  assert (h_cb_arg == 0);
}

static void
test_i ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "--i-long", 0};
  static const char *argv_1 [] = {"test", "--i-long", "i-arg", 0};
  static const char *argv_2 [] = {"test", "--i-long=i-arg-1", "i-arg", 0};
  static const char *argv_3 [] = {"test", "--i-long", "-a", 0};

  static const char *argv_4 [] = {"test", "-i", 0};
  static const char *argv_5 [] = {"test", "-i", "i-arg", 0};
  static const char *argv_6 [] = {"test", "-i", "-a", 0};

  i_flag = 0;
  i_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);
  assert (i_flag == 3);
  assert (i_cb_arg == 0);

  i_flag = 0;
  i_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);

  i_flag = 0;
  i_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == 0);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg-1") == 0);

  i_flag = 0;
  i_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_3, stderr);
  assert (sts == 0);
  assert (i_flag == 3);
  assert (i_cb_arg == 0);

  i_flag = 0;
  i_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_4, stderr);
  assert (sts == 0);
  assert (i_flag == 3);
  assert (i_cb_arg == 0);

  i_flag = 0;
  i_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_5, stderr);
  assert (sts == 0);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);

  i_flag = 0;
  i_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_6, stderr);
  assert (sts == 0);
  assert (i_flag == 3);
  assert (i_cb_arg == 0);
}


static void
test_j ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-j", 0};
  static const char *argv_1 [] = {"test", "-j", "j-arg", 0};
  static const char *argv_2 [] = {"test", "-j", "-a", 0};

  j_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == -1);
  assert (j_cb_arg == 0);

  j_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0);
  assert (j_cb_arg != 0 && strcmp (j_cb_arg, "j-arg") == 0);

  j_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == -1);
  assert (j_cb_arg == 0);
}

static void
test_k ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "--k-long", 0};
  static const char *argv_1 [] = {"test", "--k-long", "k-arg", 0};
  static const char *argv_2 [] = {"test", "--k-long=k-arg-1", "k-arg", 0};
  static const char *argv_3 [] = {"test", "--k-long", "-a", 0};

  k_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == -1);
  assert (k_cb_arg == 0);

  k_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0);
  assert (k_cb_arg != 0 && strcmp (k_cb_arg, "k-arg") == 0);

  k_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == 0);
  assert (k_cb_arg != 0 && strcmp (k_cb_arg, "k-arg-1") == 0);

  k_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_3, stderr);
  assert (sts == -1);
  assert (k_cb_arg == 0);
}

static void
test_l ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "--l-long", 0};
  static const char *argv_1 [] = {"test", "--l-long", "l-arg", 0};
  static const char *argv_2 [] = {"test", "--l-long=l-arg-1", "l-arg", 0};
  static const char *argv_3 [] = {"test", "--l-long", "-a", 0};

  static const char *argv_4 [] = {"test", "-l", 0};
  static const char *argv_5 [] = {"test", "-l", "l-arg", 0};
  static const char *argv_6 [] = {"test", "-l", "-a", 0};

  l_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == -1);
  assert (l_cb_arg == 0);

  l_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0);
  assert (l_cb_arg != 0 && strcmp (l_cb_arg, "l-arg") == 0);

  l_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == 0);
  assert (l_cb_arg != 0 && strcmp (l_cb_arg, "l-arg-1") == 0);

  l_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_3, stderr);
  assert (sts == -1);
  assert (l_cb_arg == 0);

  l_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_4, stderr);
  assert (sts == -1);
  assert (l_cb_arg == 0);

  l_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_5, stderr);
  assert (sts == 0);
  assert (l_cb_arg != 0 && strcmp (l_cb_arg, "l-arg") == 0);

  l_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_6, stderr);
  assert (sts == -1);
  assert (l_cb_arg == 0);
}

static void
test_m ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "--m-long", 0};
  static const char *argv_1 [] = {"test", "--m-long", "m-arg", 0};
  static const char *argv_2 [] = {"test", "--m-long=m-arg-1", "m-arg", 0};
  static const char *argv_3 [] = {"test", "--m-long", "-a", 0};

  static const char *argv_4 [] = {"test", "-m", 0};
  static const char *argv_5 [] = {"test", "-m", "m-arg", 0};
  static const char *argv_6 [] = {"test", "-m", "-a", 0};

  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == -1);
  assert (m_flag == 0);
  assert (m_cb_arg == 0);

  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0);
  assert (m_flag == 4);
  assert (m_cb_arg != 0 && strcmp (m_cb_arg, "m-arg") == 0);

  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == 0);
  assert (m_flag == 4);
  assert (m_cb_arg != 0 && strcmp (m_cb_arg, "m-arg-1") == 0);

  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_3, stderr);
  assert (sts == -1);
  assert (m_flag == 0);
  assert (m_cb_arg == 0);

  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 2, argv_4, stderr);
  assert (sts == -1);
  assert (m_flag == 0);
  assert (m_cb_arg == 0);

  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_5, stderr);
  assert (sts == 0);
  assert (m_flag == 4);
  assert (m_cb_arg != 0 && strcmp (m_cb_arg, "m-arg") == 0);

  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_6, stderr);
  assert (sts == -1);
  assert (m_flag == 0);
  assert (m_cb_arg == 0);
}


static void
test_adi ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-aid", 0};
  static const char *argv_1 [] = {"test", "-adi", 0};
  static const char *argv_2 [] = {"test", "-iad", 0};
  static const char *argv_3 [] = {"test", "-ida", 0};
  static const char *argv_4 [] = {"test", "-dai", 0};
  static const char *argv_5 [] = {"test", "-dia", 0};

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 2, argv_1, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 2, argv_2, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 2, argv_3, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 2, argv_4, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 2, argv_5, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);
}

static void
test_adi_arg ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-aid", "i-arg", 0};
  static const char *argv_1 [] = {"test", "-adi", "i-arg", 0};
  static const char *argv_2 [] = {"test", "-iad", "i-arg", 0};
  static const char *argv_3 [] = {"test", "-ida", "i-arg", 0};
  static const char *argv_4 [] = {"test", "-dai", "i-arg", 0};
  static const char *argv_5 [] = {"test", "-dia", "i-arg", 0};

  d_flag = 0;
  i_flag = 0;
  i_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_0, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 3, argv_3, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 3, argv_4, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);

  d_flag = 0;
  i_flag = 0;
  sts = ulib_options_parse (options, 3, argv_5, stderr);
  assert (sts == 0);
  assert (d_flag == 1);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);
}

static void
test_aim_arg ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-aim", "i-arg", 0};
  static const char *argv_1 [] = {"test", "-ami", "m-arg", 0};
  static const char *argv_2 [] = {"test", "-iam", "i-arg", 0};
  static const char *argv_3 [] = {"test", "-ima", "i-arg", 0};
  static const char *argv_4 [] = {"test", "-mai", "m-arg", 0};
  static const char *argv_5 [] = {"test", "-mia", "m-arg", 0};

  i_flag = 0;
  i_cb_arg = 0;
  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_0, stderr);
  assert (sts == -1);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);
  assert (m_flag == 0);
  assert (m_cb_arg == 0);

  i_flag = 0;
  i_cb_arg = 0;
  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_1, stderr);
  assert (sts == 0 );
  assert (i_flag == 3);
  assert (i_cb_arg == 0);
  assert (m_flag == 4);
  assert (m_cb_arg != 0 && strcmp (m_cb_arg, "m-arg") == 0);

  i_flag = 0;
  i_cb_arg = 0;
  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_2, stderr);
  assert (sts == -1);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);
  assert (m_flag == 0);
  assert (m_cb_arg == 0);

  i_flag = 0;
  i_cb_arg = 0;
  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_3, stderr);
  assert (sts == -1);
  assert (i_flag == 3);
  assert (i_cb_arg != 0 && strcmp (i_cb_arg, "i-arg") == 0);
  assert (m_flag == 0);
  assert (m_cb_arg == 0);

  i_flag = 0;
  i_cb_arg = 0;
  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_4, stderr);
  assert (sts == 0 );
  assert (i_flag == 3);
  assert (i_cb_arg == 0);
  assert (m_flag == 4);
  assert (m_cb_arg != 0 && strcmp (m_cb_arg, "m-arg") == 0);

  i_flag = 0;
  i_cb_arg = 0;
  m_flag = 0;
  m_cb_arg = 0;
  sts = ulib_options_parse (options, 3, argv_5, stderr);
  assert (sts == 0 );
  assert (i_flag == 3);
  assert (i_cb_arg == 0);
  assert (m_flag == 4);
  assert (m_cb_arg != 0 && strcmp (m_cb_arg, "m-arg") == 0);
}


static struct ulib_option options_no_cb [] =
{
  {
    .key = 'n',
    .name = "n-long",
    .flags = ulib_option_required_arg
  },


  {
    .key = '\0'
  }
};


static void
test_errors ()
{
  volatile int sts;
  static const char *argv_0 [] = {"test", "-z", 0};
  static const char *argv_1 [] = {"test", "-az", 0};
  static const char *argv_2 [] = {"test", "-za", 0};
  static const char *argv_3 [] = {"test", "-a", "--z-long", 0};
  static const char *argv_4 [] = {"test", "--z-long", "--b-long", 0};
  static const char *argv_5 [] = {"test", "--z-long=z-arg", "--b-long", 0};
  static const char *argv_6 [] = {"test", "--z-long=z-arg", "z-arg", 0};

  static const char *argv_7 [] = {"test", "-n", "n-arg", 0 };
  static const char *argv_8 [] = {"test", "--n-long", "n-arg", 0 };
  static const char *argv_9 [] = {"test", "--n-long=n-arg-1", "n-arg", 0 };
  static const char *argv_10 [] = {"test", "nonopt-arg", 0 };

  sts = ulib_options_parse (options, 2, argv_0, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options, 2, argv_1, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options, 2, argv_2, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options, 3, argv_3, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options, 3, argv_4, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options, 3, argv_5, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options, 3, argv_6, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options_no_cb, 3, argv_7, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options_no_cb, 3, argv_8, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options_no_cb, 3, argv_9, stderr);
  assert (sts == -1);

  sts = ulib_options_parse (options_no_cb, 2, argv_10, stderr);
  assert (sts == 0);
}


static const char *cmdlines [][15] =
{
  {"test", "--h-long=", "-j", "j-arg-value-1", "--b-long", 0, },
  {"test", "-c", "--g-long", "g-arg-value-2", "--g-long", "g-arg-value-2", 0, },
  {"test", "--g-long", "g-arg-value-2", 0, },
  {"test", "--b-long", 0, },
  {"test", "-j", "j-arg-value-2", "-a", "-j", "j-arg-value-1", 0, },
  {"test", "-c", "-j", "j-arg-value-1", 0, },
  {"test", "--e-long", 0, },
  {"test", "--i-long=i-arg-value-2", 0, },
  {"test", "-e", "--i-long=i-arg-value-1", 0, },
  {"test", "-d", 0, },
  {"test", "--h-long=h-arg-value-1", 0, },
  {"test", "--m-long", "m-arg-value-1", "--c-long", 0, },
  {"test", "-f", 0, },
  {"test", "-m", "m-arg-value-2", "--m-long", "m-arg-value-1", 0, },
  {"test", "--h-long", "h-arg-value-2", "--l-long", "l-arg-value-1",
   "--d-long", "--g-long", "g-arg-value-1", "-a", "--m-long=m-arg-value-2",
   "-f", "f-arg-value-1", 0, },
  {"test", "--k-long=k-arg-value-2", "--l-long=l-arg-value-2", 0, },
  {"test", "--i-long", "i-arg-value-2", "--e-long", "--b-long", 0, },
  {"test", "-c", "--g-long", "g-arg-value-1", 0, },
  {"test", "-a", 0, },
  {"test", "-e", "--e-long", 0, },
  {"test", "--k-long", "k-arg-value-1", 0, },
  {"test", "--l-long", "l-arg-value-1", "-a", 0, },
  {"test", "-m", "m-arg-value-1", 0, },
  {"test", "-h", "h-arg-value-2", "-m", "m-arg-value-2", 0, },
  {"test", "--b-long", 0, },
  {"test", "--m-long", "m-arg-value-2", "--m-long=m-arg-value-2", "-m",
   "m-arg-value-1", 0, },
  {"test", "--i-long", "i-arg-value-2", "-e", 0, },
  {"test", "--k-long", "k-arg-value-1", "--b-long", 0, },
  {"test", "--m-long=m-arg-value-1", "--l-long=l-arg-value-2", 0, },
  {"test", "-j", "j-arg-value-2", 0, },
  {"test", "--k-long", "k-arg-value-1", 0, },
  {"test", "--c-long", "--b-long", "--d-long", 0, },
  {"test", "--c-long", 0, },
  {"test", "-c", 0, },
  {"test", "-h", "h-arg-value-1", "--h-long", "h-arg-value-2", "--k-long",
   "k-arg-value-1", 0, },
  {"test", "-f", "f-arg-value-2", 0, },
  {"test", "--i-long=i-arg-value-2", "-a", "--h-long", 0, },
  {"test", "-j", "j-arg-value-1", 0, },
  {"test", "-j", "j-arg-value-2", "--g-long", "g-arg-value-1", "-e", "-f",
   "f-arg-value-2", 0, },
  {"test", "--i-long", 0, },
  {"test", "-f", "-j", "j-arg-value-1", "--b-long", "--c-long", "--b-long",
   "--b-long", "-c", 0, },
  {"test", "-m", "m-arg-value-2", 0, },
  {"test", "--e-long", "-d", 0, },
  {"test", "--g-long", "g-arg-value-1", 0, },
  {"test", "--h-long", "h-arg-value-1", 0, },
  {"test", "-f", "f-arg-value-1", 0, },
  {"test", "--h-long=h-arg-value-2", 0, },
  {"test", "-f", "f-arg-value-2", 0, },
  {"test", "-a", "--l-long", "l-arg-value-1", 0, },
  {"test", "--g-long", "g-arg-value-1", 0, },
  {"test", "-l", "l-arg-value-2", 0, },
  {"test", "-j", "j-arg-value-1", "-f", 0, },
  {"test", "-a", 0, },
  {"test", "--i-long", "i-arg-value-2", "-a", 0, },
  {"test", "--b-long", "-a", "-m", "m-arg-value-1", "-f", "f-arg-value-1", 0, },
  {"test", "--h-long=h-arg-value-2", "-i", "--m-long", "m-arg-value-1", "-h",
   0, },
  {"test", "--d-long", 0, },
  {"test", "--h-long=", "--b-long", "-f", "f-arg-value-1", "--g-long",
   "g-arg-value-1", 0, },
  {"test", "--b-long", "--k-long", "k-arg-value-1", "-j", "j-arg-value-1",
   "--c-long", 0, },
  {"test", "--b-long", "-a", 0, },
  {"test", "--i-long", "-m", "m-arg-value-2", 0, },
  {"test", "--b-long", "--g-long", "--e-long", "--m-long=m-arg-value-2", 0, },
  {"test", "--b-long", "-c", "--k-long", "k-arg-value-1", 0, },
  {"test", "-h", "h-arg-value-1", 0, },
  {"test", "-d", 0, },
  {"test", "-m", "m-arg-value-1", 0, },
  {"test", "--c-long", "--l-long=l-arg-value-2", 0, },
  {"test", "--e-long", 0, },
  {"test", "--c-long", 0, },
  {"test", "-c", "-a", 0, },
  {"test", "--i-long", "i-arg-value-2", 0, },
  {"test", "--m-long", "m-arg-value-1", "-j", "j-arg-value-1", "-f",
   "f-arg-value-2", "-l", "l-arg-value-2", 0, },
  {"test", "-f", "f-arg-value-2", 0, },
  {"test", "--k-long", "k-arg-value-1", 0, },
  {"test", "--h-long", "h-arg-value-1", "--i-long", 0, },
  {"test", "--e-long", "--i-long=", 0, },
  {"test", "-f", "f-arg-value-2", "--l-long", "l-arg-value-2", 0, },
  {"test", "--c-long", "--b-long", "--c-long", "--m-long=m-arg-value-1",
   "--c-long", 0, },
  {"test", "-a", "--l-long=l-arg-value-2", 0, },
  {"test", "-a", 0, },
  {"test", "--b-long", "--k-long", "k-arg-value-1", 0, },
  {"test", "--m-long", "m-arg-value-2", "--i-long=i-arg-value-1", "--e-long",
   "--e-long", 0, },
  {"test", "--g-long", "g-arg-value-2", "--l-long", "l-arg-value-1", 0, },
  {"test", "--m-long=m-arg-value-1", 0, },
  {"test", "-i", "-f", "f-arg-value-2", "--d-long", 0, },
  {"test", "--h-long=", 0, },
  {"test", "-m", "m-arg-value-2", 0, },
  {"test", "-d", 0, },
  {"test", "--h-long=", "--b-long", 0, },
  {"test", "--k-long", "k-arg-value-2", "-h", "h-arg-value-2", 0, },
  {"test", "-j", "j-arg-value-1", 0, },
  {"test", "-j", "j-arg-value-2", "--l-long", "l-arg-value-1", 0, },
  {"test", "--m-long=m-arg-value-1", "--b-long", 0, },
  {"test", "-a", "-c", 0, },
  {"test", "--k-long=k-arg-value-2", 0, },
  {"test", "--h-long", "h-arg-value-2", 0, },
  {"test", "-f", 0, },
  {"test", "--e-long", "--d-long", "-e", "--k-long", "k-arg-value-1", 0, },
  {"test", "--m-long", "m-arg-value-2", "--l-long", "l-arg-value-1",
   "--c-long", 0, },
  {"test", "-e", 0, },
  {"test", "-d", "-c", 0, },
  {"test", "--d-long", 0, },
  {"test", "-c", "-f", "f-arg-value-2", "-a", "--l-long", "l-arg-value-2",
   "-m", "m-arg-value-2", "--l-long=l-arg-value-1", "--h-long=h-arg-value-1",
   0, },
  {0, }
};

static void
test_random_cmdlines ()
{
  volatile int sts;
  int argc;
  const char * (*argv) [15];

  argv = &cmdlines [0];

  while ((*argv) [0])
    {
      argc = 0;
      while ((*argv) [argc])
        ++argc;

      sts = ulib_options_parse (options, argc, *argv, stderr);
      assert (sts == 0);

      ++argv;
    }
}


static const ulib_option smk_options [] =
{
  { .key = 'a', .name = "all",
    .help = "\t\t\tUnconditionally rebuild targets" },

  { .key = 'c', .name = "clean",
    .help = "\t\t\tClean the project" },

  { .key = 'C', .name = "realclean",
    .help = "\t\tReally clean the project" },

  { .key = 'f', .name = "file", .flags = ulib_option_required_arg,
    .arg = "<name>", .help = "\tName of the makefile" },

  { .key = 'h', .name = "help",
    .help = "\t\t\tDisplay usage information" },

  { .key = 'i', .name = "install",
    .help = "\t\tInstall the package" },

  { .key = 'j', .name = "jobs", .flags = ulib_option_required_arg,
    .arg = "<num>", .help = "\tRun multiple jobs in parallel" },

  { .key = 'k', .name = "keep-going",
    .help = "\t\tKeep going when some targets can't be made" },

  { .key = 'n', .name = "dry-run",
    .help = "\t\tDo not execute commands" },

  { .key = 'o', .name = "output", .flags = ulib_option_required_arg,
    .arg = "<name>", .help = "\tSpecify top level build directory" },

  { .key = 'p', .name = "prefix", .flags = ulib_option_required_arg,
    .arg = "<path>",.help = "\tSpecify installation prefix" },

  { .key = 'q', .name = "quiet",
    .help = "\t\t\tSuppress progress messages" },

  { .key = 's', .name = "subdir", .flags = ulib_option_required_arg,
    .arg = "<name>", .help = "\tSpecify build subdirectory" },

  { .key = 'u', .name = "uninstall",
    .help = "\t\tUninstall the package" },

  { .key = 'V', .name = "version",
    .help = "\t\tDisplay version" },

  { .key = 'v', .name = "verbose",
    .help = "\t\tVerbose messages" },

  { .key = 'x', .name = "xml",
    .help = "\t\t\tGenerate XML report" },

  { .key = 0, }
};

static void
test_option_help ()
{
  ulib_options_help (stderr, smk_options);
}

int
main ()
{

  test_a ();
  test_b ();
  test_c ();
  test_d ();
  test_e ();
  test_f ();
  test_g ();
  test_h ();
  test_i ();
  test_j ();
  test_k ();
  test_l ();
  test_m ();
  test_adi ();
  test_adi_arg ();
  test_aim_arg ();
  test_errors ();
  test_random_cmdlines ();
  test_option_help ();

  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 91148862-0ef2-4d2d-9d1f-cb2e3fd9de74
 * End:
 */
