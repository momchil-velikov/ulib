#ifndef ulib__test_object_h
#define ulib__test_object_h 1

#include <ulib/object.h>

BEGIN_DECLS

/*
 * Interface I
 */

/* Get unique interface identifier.  */
extern unsigned int I__type ();
#define I_TYPE (I__type ())

/* Forward declarations of a dummy instance type and a interface
   info/virtual table type.  */
typedef struct I I;
typedef struct IIface IIface;

/* Cast to the dummy instance type.  */
#define I_CAST(OBJ) ULIB_IFACE_CAST (I, I_TYPE, (OBJ))

/* Get interface info/virtual methods table.  */
#define I_IFACE(OBJ) ULIB_GET_OBJ_IFACE (IIface, I_TYPE, (OBJ))

/* Declare: an interface info structure, which includes the virtual
   methods table.  */
struct IIface
{
  /* Interface info struct must begin with the generic ``ulib_iface''
     structure.  */
  ulib_iface iface;

  /* Methods follow.  */
  int (*peek) (const I *);
  void (*poke) (I *, int);
};

/* Define ``interface'' functions, the ones the user of the object
   invokes.  It's a good idea to have them inlined.  */
static inline int
I_peek (const I *self)
{
  return I_IFACE (self)->peek (self);
}

static inline void
I_poke (I *self, int v)
{
  I_IFACE (self)->poke (self, v);
}


typedef struct A A;
typedef struct AClass AClass;

/* Get unique class identifier.  */
extern unsigned int A__type ();
#define A_TYPE (A__type ())

/* Instance cast.  */
#define A_CAST(OBJ) ULIB_INSTANCE_CAST (A, A_TYPE, (OBJ))

/* Get class/virtual table.  */
#define A_CLASS(OBJ) ULIB_GET_CLASS (AClass, A_TYPE, (OBJ))

/* Class/virtual table cast.  */
#define A_CLASS_CAST(CLS) ULIB_CLASS_CAST (AClass, A_TYPE, (CLS))

/* Declare layout and the name of the class.  */
struct A
{
  /* Instances must begin with the superclass subobject.  */
  ulib_object super;

  /* Class members follow.  */
  int a;
};

/* Declare a class info structure, which includes the virtual methods
   table.  */
struct AClass
{
  /* Class info struct must begin with the class info struct of the
     superclass.  */
  ulib_class super;

  /* Methods follow.  */
  int (*get) (const A *);
  void (*set) (A*, int);
};

/* Declare a instance creation function(s), with arbitrary names and
   parameters.  */
extern A *A_create (int);

/* Declare and/or define ``interface'' functions, the ones the user of
   the object invokes.  It's a good idea to have them inlined.  */
static inline int
A_get (const A *self)
{
  return A_CLASS (self)->get (self);
}

static inline void
A_set (A *self, int v)
{
  A_CLASS (self)->set (self, v);
}


typedef struct B B;
typedef struct BClass BClass;

/* Get unique class identifier.  */
extern unsigned int B__type ();
#define B_TYPE (B__type ())

/* Instance cast.  */
#define B_CAST(OBJ) ULIB_INSTANCE_CAST (B, B_TYPE, (OBJ))

/* Get class/virtual table.  */
#define B_CLASS(OBJ) ULIB_GET_CLASS (BClass, B_TYPE, (OBJ))

/* Class/virtual table cast.  */
#define B_CLASS_CAST(CLS) ULIB_CLASS_CAST (BClass, B_TYPE, (CLS))

/* Declare layout and the name of the class.  */
struct B
{
  /* Instances must begin with the superclass subobject.  */
  A super;

  /* Class members follow.  */
  int b;
};

/* Declare a class info structure, which includes the virtual methods
   table. */
struct BClass
{
  /* Class info struct must begin with the class info struct of the
     superclass.  */
  AClass super;
};

/* Declare a instance creation function(s), with arbitrary names and
   parameters.  */
extern B *B_create (void);

END_DECLS

#endif /*  ulib__test_object_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 20b85a15-56e9-4fa8-b452-41d3d0a81120
 * End:
 */
