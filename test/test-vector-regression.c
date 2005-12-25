#include <ulib/vector.h>
#include <assert.h>

static void
test_1 ()
{
  int sts;
  unsigned int n;
  unsigned int *p;
  ulib_vector v;

  sts = ulib_vector_init (&v, ULIB_ELT_SIZE, sizeof (unsigned int), 0);
  assert (sts == 0);

  n = 1;
  sts = ulib_vector_append (&v, &n);
  assert (sts == 0);

  n = 2;
  sts = ulib_vector_append (&v, &n);
  assert (sts == 0);

  n = 3;
  sts = ulib_vector_append (&v, &n);
  assert (sts == 0);

  n = 4;
  sts = ulib_vector_insert (&v, 0, &n);
  assert (sts == 0);

  n = 5;
  sts = ulib_vector_insert (&v, 1, &n);
  assert (sts == 0);

  assert (ulib_vector_length (&v) == 5);

  p = ulib_vector_front (&v);
  assert (p [0] == 4 && p [1] == 5 && p [2] == 1 && p [3] == 2 && p [4] == 3);

  ulib_vector_destroy (&v);
}

static void
test_2()
{
  int sts;
  unsigned int n;
  unsigned int *p;
  ulib_vector v;

  sts = ulib_vector_init (&v, ULIB_ELT_SIZE, sizeof (unsigned int), 0);
  assert (sts == 0);

  n = 1;
  sts = ulib_vector_append (&v, &n);
  assert (sts == 0);

  n = 2;
  sts = ulib_vector_append (&v, &n);
  assert (sts == 0);

  n = 3;
  sts = ulib_vector_append (&v, &n);
  assert (sts == 0);

  n = 4;
  sts = ulib_vector_append (&v, &n);
  assert (sts == 0);

  ulib_vector_remove (&v, 1);

  assert (ulib_vector_length (&v) == 3);

  p = ulib_vector_front (&v);
  assert (p [0] == 1 && p [1] == 3 && p [2] == 4);
}

int
main ()
{
  test_1 ();
  test_2 ();
  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 4dfeba13-764d-487e-9859-66a7db7e2ca3
 * End:
 */
