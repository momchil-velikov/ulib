#ifndef ulib__object_h
#define ulib__object_h 1

#include "defs.h"
#include "assert.h"

BEGIN_DECLS

/* Interface descriptor.  */
struct ulib_iface
{
  /* Interface ID.  */
  unsigned int id;

  /* Virtual methods follow.  */
};
typedef struct ulib_iface ulib_iface;

/* Class descriptor.  */
struct ulib_class
{
  /* Identifier.  */
  unsigned int id;

  /* Superclass.  */
  const struct ulib_class *super;

  /* Size of the interface table.  */
  unsigned int isize;

  /* Interface table.  */
  const ulib_iface **itbl;

  /* Virtual methods follow.  */
};
typedef struct ulib_class ulib_class;

/* Instance.  */
struct ulib_object
{
  const struct ulib_class *cls;
};
typedef struct ulib_object ulib_object;

/* Class init function type.  */
typedef void (*ulib_class_init) (ulib_class *);

/* Instance init function type.  */
typedef void (*ulib_inst_init) (ulib_object *);

/* Instance destroy function.  */
typedef void (*ulib_inst_destroy) (ulib_object *);

/* Class registry.  */
struct ulib_classreg
{
  /* Superclass ID. */
  unsigned int super;

  /* Class info */
  ulib_class *info;

  /* Class init function.  */
  ulib_class_init class_init;

  /* Instance init function.  */
  ulib_inst_init inst_init;

  /* Instance destroy function.  */
  ulib_inst_destroy inst_destroy;
};

/* Register an interface. Return an unique type ID.  */
unsigned int ulib_iface_register (void);

/* Regiser an interface functions table.  */
void ulib_iface_add (ulib_class *cls, const ulib_iface *);

/* Register a class. Return an unique type ID.  */
unsigned int ulib_class_register (unsigned int super, ulib_class *info,
				  ulib_class_init class_init,
				  ulib_inst_init inst_init,
				  ulib_inst_destroy inst_destroy);

/* Retrieve a class info by type ID.  */
const ulib_class *ulib_class_lookup (unsigned int);

/* Initialize an instance.  */
void ulib_object_init (ulib_object *obj, unsigned int id);

/* Destroy an instance.  */
void ulib_object_destroy (ulib_object *obj);

/* Check whether CLS is derived from class ID.  */
int ulib_check_derived (const ulib_class *cls, unsigned int id);

/* Check whether OBJ is an instance of CLS.  */
static inline int
ulib_instance_of (const ulib_object *obj, unsigned int id)
{
  return ulib_check_derived (obj->cls, id);
}

/* Cast a class info to superclass info.  */
#if ENABLE_CHECKS
# define ULIB_CLASS_CAST(SUPER, TYPE, DERIVED)				\
    (ulib_assert							\
     (ulib_check_derived ((const ulib_class *) (DERIVED), (TYPE))),	\
     (SUPER *) (DERIVED))
#else
# define ULIB_CLASS_CAST(SUPER, TYPE, DERIVED)	\
    ((SUPER *) (DERIVED))
#endif

/* Get a pointer to the class info for the OBJ, cast to CLS.  */
#define ULIB_OBJ_CLASS(OBJ)			\
  (((const ulib_object *) (OBJ))->cls)
#define ULIB_GET_CLASS(CLS, TYPE, OBJ)			\
  ULIB_CLASS_CAST (CLS, (TYPE), ULIB_OBJ_CLASS (OBJ))

/* Get superclass info.  */
#define ULIB_SUPERCLASS(CLS)			\
  (((const ulib_class *) (CLS))->super)
#define ULIB_OBJ_SUPERCLASS(OBJ)		\
  ULIB_SUPERCLASS (ULIB_OBJ_CLASS (OBJ))

#define ULIB_GET_SUPERCLASS(SUPER, TYPE, CLS)		\
  (ulib_assert (ulib_check_derived ((CLS), (TYPE))),	\
   (SUPER *) ulib_class_lookup (TYPE))
#define ULIB_GET_OBJ_SUPERCLASS(SUPER, TYPE, OBJ)			\
  ULIB_GET_SUPERCLASS (SUPER, (TYPE), ULIB_OBJ_SUPERCLASS (OBJ))

/* Get a pointer to OBJ, cast (optionally checked) to CLS.  */
#if ENABLE_CHECKS
# define ULIB_INSTANCE_CAST(CLS, TYPE, OBJ)				   \
    (ulib_assert (ulib_instance_of ((const ulib_object *) (OBJ), (TYPE))), \
     (CLS *) (OBJ))
#else
# define ULIB_INSTANCE_CAST(CLS, TYPE, OBJ)	\
    ((CLS *) (OBJ))
#endif

const void *ulib__find_iface_slow (unsigned int id, unsigned int idx,
				   const ulib_class *cls);

static inline const void *
ulib__find_iface_fast (unsigned int id, const ulib_class *cls)
{
  unsigned int idx;

  idx = id & (cls->isize - 1);
  if (cls->itbl && cls->itbl [idx] && cls->itbl [idx]->id == id)
    return cls->itbl [idx];

  return ulib__find_iface_slow (id, idx, cls);
}

/* Get interface table.  */
#define ULIB_IFACE(ID, CLS)			\
  (ulib__find_iface_fast ((ID), (const ulib_class *) (CLS)))
#define ULIB_OBJ_IFACE(ID, OBJ)				\
  (ulib__find_iface_fast ((ID), ULIB_OBJ_CLASS (OBJ)))

/* Check whether CLS/OBJ implements interface ID.  */
#define ULIB_IMPLEMENTS(ID, CLS)		\
  (ULIB_IFACE((ID), (CLS)) != 0)
#define ULIB_OBJ_IMPLEMENTS(ID, OBJ)		\
  (ULIB_OBJ_IFACE((ID), (OBJ)) != 0)

/* Cast a OBJ to a pointer to a IFACE, with implements check.  */
#if ENABLE_CHECKS
# define ULIB_IFACE_CAST(IFACE, ID, OBJ)		\
    (ulib_assert (ULIB_OBJ_IMPLEMENTS ((ID), (OBJ))),	\
     (IFACE *) (OBJ))
#else
# define ULIB_IFACE_CAST(IFACE, ID, OBJ)		\
    ((IFACE *) (OBJ))
#endif

#if ENABLE_CHECKS
# define ULIB_GET_IFACE(IFACE, ID, CLS)			\
    (ulib_assert (ULIB_IMPLEMENTS ((ID), (CLS))),	\
     (IFACE *) ULIB_IFACE ((ID), (CLS)))
#else
# define ULIB_GET_IFACE(IFACE, ID, CLS)			\
    ((IFACE *) ULIB_IFACE ((ID), (CLS)))
#endif

#if ENABLE_CHECKS
# define ULIB_GET_OBJ_IFACE(IFACE, ID, OBJ)			\
    (ulib_assert (ULIB_IMPLEMENTS((ID), ULIB_OBJ_CLASS (OBJ))),	\
     (IFACE *) ULIB_OBJ_IFACE ((ID), (OBJ)))
#else
# define ULIB_GET_OBJ_IFACE(IFACE, ID, OBJ)			\
    ((IFACE *) ULIB_OBJ_IFACE ((ID), (OBJ)))
#endif

END_DECLS

#endif /* ulib__object_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 4aa41419-e9cc-4ba9-8d70-47c3d89a0c16
 * End:
 */
