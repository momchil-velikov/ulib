#include "object.h"
#include "alloc.h"
#include "vector.h"

/* Check whether CLS is derived from class ID.  */
int
ulib_check_derived (const ulib_class *cls, unsigned int id)
{
  do
    {
      if (cls->id == id)
	return 1;

      cls = cls->super;
    }
  while (cls);

  return id == 0;
}

/* Register an interface type. Return an unique interface id.  */
unsigned int
ulib_iface_register (void)
{
  static unsigned int id = 0;
  return ++id;
}

/* Register an interface table.  Increase the hash table size and
   rehash interfaces if not enough space available.  */
static int
hash_iface (unsigned int n, const ulib_iface **itbl, const ulib_iface *iface)
{
  unsigned int i, idx;

  i = 0;
  idx = iface->id & (n - 1);
  do
    {
      if (itbl [idx] == 0 || itbl [idx]->id == iface->id)
	{
	  itbl [idx] = iface;
	  return 0;
	}

      idx = (idx + 1) & (n - 1);
    }
  while (++i < n);

  return -1;
}

void
ulib_iface_add (struct ulib_class *cls, const ulib_iface *iface)
{
  unsigned int i;
  const ulib_iface **ntbl;

  if (cls->isize == 0)
    {
      cls->isize = 4;
      cls->itbl = ulib_calloc (cls->isize, sizeof (ulib_iface *));
    }

  if (hash_iface (cls->isize, cls->itbl, iface))
    {
      ntbl = ulib_calloc (cls->isize * 2, sizeof (ulib_iface *));
      for (i = 0; i < cls->isize; i++)
	if (cls->itbl [i])
	  hash_iface (cls->isize * 2, ntbl, cls->itbl [i]);
      ulib_free (cls->itbl);
      cls->itbl = ntbl;
    }
}

const void *
ulib__find_iface_slow (unsigned int id, unsigned int idx,
		       const ulib_class *cls)
{
  unsigned int i;

  if (cls->isize == 0)
    return 0;

  i = 0;
  do
    {
      idx = (idx + 1) & (cls->isize - 1);
      if (cls->itbl [idx] && cls->itbl [idx]->id == id)
	return cls->itbl [idx];
    }
  while (++i < cls->isize - 1);
  return 0;
}

/* Class registry vector. */
static ulib_vector class_registry;

/* Invoke class initialization functions.  */
static void
invoke_class_init (ulib_class *cls, unsigned int id)
{
  const struct ulib_classreg *reg;

  if (id != 0)
    {
      reg = ulib_vector_elt (&class_registry, id - 1);
      invoke_class_init (cls, reg->super);
      if (reg->class_init)
	reg->class_init (cls);
    }
}

/* Invoke instance initialization functions.  */
static void
invoke_inst_init (ulib_object *obj, unsigned int id)
{
  const struct ulib_classreg *reg;

  if (id != 0)
    {
      reg = ulib_vector_elt (&class_registry, id - 1);
      invoke_inst_init (obj, reg->super);
      if (reg->inst_init)
	reg->inst_init (obj);
    }
}

/* Invoke instance destroy functions.  */
static void
invoke_inst_destroy (ulib_object *obj, unsigned int id)
{
  const struct ulib_classreg *reg;

  if (id != 0)
    {
      reg = ulib_vector_elt (&class_registry, id - 1);
      invoke_inst_destroy (obj, reg->super);
      if (reg->inst_destroy)
	reg->inst_destroy (obj);
    }
}

/* Register a class. Return a unique type ID.  */
unsigned int
ulib_class_register (unsigned int super, ulib_class *info,
		     ulib_class_init class_init, ulib_inst_init inst_init,
		     ulib_inst_destroy inst_destroy)

{
  struct ulib_classreg reg;;
  unsigned int id;

  /* Initialize registry.  */
  static int initialized = 0;
  if (!initialized)
    {
      ulib_vector_init (&class_registry,
			ULIB_ELT_SIZE, sizeof (struct ulib_classreg),
			ULIB_GROWTH_OFFSET, 5,
			0);
      initialized = 1;
    }

  if (info->id != 0)
    return info->id;

  /* Register class.  */
  reg.super = super;
  reg.info = info;
  reg.class_init = class_init;
  reg.inst_init = inst_init;
  reg.inst_destroy = inst_destroy;

  ulib_vector_append (&class_registry, &reg);
  id = ulib_vector_length (&class_registry);

  /* Initialize class info struct.  */
  info->id = id;

  if (super == 0)
    info->super = 0;
  else
    {
      const struct ulib_classreg *regp;

      regp = ulib_vector_elt (&class_registry, super - 1);
      info->super = regp->info;
    }

  info->isize = 0;
  info->itbl = 0;
  invoke_class_init (info, id);

  return id;
}

const ulib_class *
ulib_class_lookup (unsigned int id)
{
  const struct ulib_classreg *reg;

  ulib_assert (id <= ulib_vector_length (&class_registry));
  reg = ulib_vector_elt (&class_registry, id - 1);

  return reg->info;
}

/* Initialize an instance.  */
void
ulib_object_init (ulib_object *obj, unsigned int id)
{
  const struct ulib_classreg *reg;

  reg = ulib_vector_elt (&class_registry, id - 1);
  obj->cls = reg->info;
  invoke_inst_init (obj, id);
}

/* Destroy an instance.  */
void
ulib_object_destroy (ulib_object *obj)
{
  invoke_inst_destroy (obj, obj->cls->id);
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: fb9811dc-74a2-474f-87ce-c1986cc53a77
 * End:
 */
