#include "test-object.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Interface I.
 */
unsigned int
I__type ()
{
  static unsigned int type = 0;

  if (type == 0)
    type = ulib_iface_register ();

  return type;
}


/*
 * Class A
 */

/* Define implementation functions.  */
static void
A__inst_init (A *self)
{
  self->a = 1;
}

static void
A__inst_destroy (A *self)
{
}

static int
A__get (const A *self)
{
  return self->a;
}

static void
A__set (A *self, int v)
{
  self->a = v;
}

static int
I_A__peek (const I *_self)
{
  A *self = A_CAST (_self);
  return self->a;
}

static void
I_A__poke (I *_self, int v)
{
  A *self = A_CAST (_self);
  self->a = v;
}


/* Class info.  */
static AClass a_class;

/* Implementation of interface I.  */
static IIface i_in_a;

/* Define class init function.  */
static void
A__class_init (ulib_class *_cls)
{
  AClass *cls = A_CLASS_CAST(_cls);

  cls->get = A__get;
  cls->set = A__set;

  i_in_a.iface.id = I_TYPE;
  i_in_a.peek = I_A__peek;
  i_in_a.poke = I_A__poke;
  ulib_iface_add (_cls, &i_in_a.iface);
}

/* Get the A class unique ID.  */
unsigned int
A__type ()
{
  static unsigned int type;
  if (type == 0)
    type = ulib_class_register (0, (ulib_class *) &a_class,
				(ulib_class_init) A__class_init,
				(ulib_inst_init) A__inst_init,
				(ulib_inst_destroy) A__inst_destroy);
  return type;
}


/* Define creation function.  */
A *
A_create (int v)
{
  A *obj;

  /* Allocate instance and invoke the constructor.  */
  obj = malloc (sizeof (A));
  ulib_object_init ((ulib_object *) obj, A_TYPE);
  obj->a = v;

  return obj;
}


/*
 * Class B
 */

/* Define implementation functions.  */
static void
B__inst_init (B *self)
{
}

static void
B__inst_destroy (B *self)
{
}

static int
B__get (const A *self)
{
  AClass *super;

  super = ULIB_GET_OBJ_SUPERCLASS (AClass, A_TYPE, self);
  return super->get (self) + 1;
}

static void
B__set (A *self, int v)
{
  AClass *super;

  super = ULIB_GET_OBJ_SUPERCLASS (AClass, A_TYPE, self);
  super->set (self, v + 1);
}

static void
I_B__poke (I *_self, int v)
{
  B *self = B_CAST (_self);
  self->super.a = v * 2;
}


/* Class info.  */
static BClass b_class;

/* Implementation of interface I.  */
static IIface i_in_b;

/* Define class init function.  */
static void
B__class_init (ulib_class *_cls)
{
  IIface *i;
  BClass *cls = B_CLASS_CAST (_cls);

  cls->super.get = B__get;
  cls->super.set = B__set;

  i = ULIB_GET_IFACE (IIface, I_TYPE, cls);
  i_in_b.iface.id = I_TYPE;
  i_in_b.peek = i->peek;
  i_in_b.poke = I_B__poke;
  ulib_iface_add (_cls, &i_in_b.iface);
}

/* Get the B class unique ID.  */
unsigned int
B__type ()
{
  static unsigned int type;
  if (type == 0)
    type = ulib_class_register (A_TYPE, (ulib_class *) &b_class,
				(ulib_class_init) B__class_init,
				(ulib_inst_init) B__inst_init,
				(ulib_inst_destroy) B__inst_destroy);
  return type;
}


/* Define creation function.  */
B *
B_create (void)
{
  B *obj;

  /* Allocate instance and invoke the constructor.  */
  obj = malloc (sizeof (B));
  ulib_object_init ((ulib_object *) obj, B_TYPE);

  return obj;
}


int
main ()
{
  A *a = A_create (1);
  B *b = B_create ();

  printf ("a->get() == %d, b->get() == %d\n", A_get (a), A_get (A_CAST (b)));
  printf ("a->set (2)\n"); A_set (a, 2);
  printf ("b->set (3)\n"); A_set (A_CAST (b), 3);
  printf ("a->get() == %d, b->get() == %d\n", A_get (a), A_get (A_CAST (b)));

  printf ("a->peek() == %d\n", I_peek (I_CAST (a)));
  printf ("a->poke (4)\n"); I_poke (I_CAST (a), 4);
  printf ("a->peek() == %d\n", I_peek (I_CAST (a)));

  printf ("b->peek() == %d\n", I_peek (I_CAST (b)));
  printf ("b->poke (4)\n"); I_poke (I_CAST (b), 4);
  printf ("b->peek() == %d\n", I_peek (I_CAST (b)));

  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: a7228410-8350-4c91-881e-00d4397159a3
 * End:
 */
