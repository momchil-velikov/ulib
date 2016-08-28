ULIB_STATIC ULIB_SPLAY_TREE_TYPE *
ULIB_SPLAY_TREE (splay) (ULIB_SPLAY_TREE_TYPE *t,
			 ULIB_SPLAY_TREE_KEY_TYPE k)
{
  ULIB_SPLAY_TREE_TYPE N, *l, *r, *x;

  if (t == 0)
    return 0;

  N.left = N.right = 0;
  l = r = &N;

  for (;;)
    {
      if (ULIB_SPLAY_TREE_COMPARE (k, t->key) < 0)
	{
	  x = t->left;
	  if (x == 0)
	    break;
	  if (ULIB_SPLAY_TREE_COMPARE (k, x->key) < 0)
	    {
	      t->left = x->right;
	      x->right = t;
	      t = x;
	      if (t->left == 0)
		break;
	    }
	  r->left = t;
	  r = t;
	  t = t->left;
	}
      else if (ULIB_SPLAY_TREE_COMPARE (k, t->key) > 0)
	{
	  x = t->right;
	  if (x == 0)
	    break;
	  if (ULIB_SPLAY_TREE_COMPARE (k, x->key) > 0)
	    {
	      t->right = x->left;
	      x->left = t;
	      t = x;
	      if (t->right == 0)
		break;
	    }
	  l->right = t;
	  l = t;
	  t = t->right;
	}
      else
	break;
    }

  l->right = r->left = 0;

  l->right = t->left;
  r->left = t->right;
  t->left = N.right;
  t->right = N.left;

  return t;
}

ULIB_STATIC int
ULIB_SPLAY_TREE (insert) (ULIB_SPLAY_TREE_TYPE **pt,
			  ULIB_SPLAY_TREE_TYPE *k)
{
  ULIB_SPLAY_TREE_TYPE *t;

  t = *pt;
  if (t == 0)
    {
      *pt = k;
      return 0;
    }

  t = ULIB_SPLAY_TREE (splay) (t, k->key);

  if (ULIB_SPLAY_TREE_COMPARE (k->key, t->key) < 0)
    {
      k->left = t->left;
      k->right = t;
      t->left = 0;

      *pt = k;
      return 0;
    }

  if (ULIB_SPLAY_TREE_COMPARE (k->key, t->key) > 0)
    {
      k->right = t->right;
      k->left = t;
      t->right = 0;

      *pt = k;
      return 0;
    }

  *pt = t;
  return -1;
}

ULIB_STATIC ULIB_SPLAY_TREE_TYPE *
ULIB_SPLAY_TREE (delete) (ULIB_SPLAY_TREE_TYPE **r,
			  ULIB_SPLAY_TREE_KEY_TYPE k)
{
  ULIB_SPLAY_TREE_TYPE *t, *x;

  if ((t = *r) == 0)
    return 0;

  t = ULIB_SPLAY_TREE (splay) (t, k);

  if (ULIB_SPLAY_TREE_COMPARE (t->key, k) == 0)
    {
      if (t->left == 0)
	x = t->right;
      else
	{
	  x = ULIB_SPLAY_TREE (splay) (t->left, k);
	  x->right = t->right;
	}

      *r = x;

      t->left = t->right = 0;
      return t;
    }
  else
    {
      *r = t;
      return 0;
    }
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
