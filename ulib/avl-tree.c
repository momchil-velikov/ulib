ULIB_STATIC int
ULIB_AVL_TREE (insert) (ULIB_AVL_TREE_TYPE **pr, ULIB_AVL_TREE_TYPE *key)
{
  ULIB_AVL_TREE_TYPE *t, *r;
  int h;

  r = *pr;
  if (r == 0)
    {
      key->left = key->right = 0;
      key->balance = 0;
      *pr = key;
      return 1;
    }

  if (ULIB_AVL_TREE_COMPARE (r->key, key->key) > 0)
    {
      if ((h = ULIB_AVL_TREE (insert) (&r->left, key)) <= 0)
	return h;

      switch (r->balance)
	{
	case -1:
	  t = r->left;
	  if (t->balance == -1)
	    {
	      r->left = t->right;
	      t->right = r;
	      t->right->balance = 0;
	    }
	  else
	    {
	      t = t->right;
	      r->left->right = t->left;
	      t->left = r->left;
	      r->left = t->right;
	      t->right = r;

	      if (t->balance == -1)
		t->right->balance = 1;
	      else
		t->right->balance = 0;

	      if (t->balance == 1)
		t->left->balance = -1;
	      else
		t->left->balance = 0;
	    }
	  t->balance = 0;
	  *pr = t;
	  return 0;

	case 0:
	  r->balance = -1;
	  return 1;

	case 1:
	  r->balance = 0;
	  return 0;
	}
    }

  if (ULIB_AVL_TREE_COMPARE (r->key, key->key) < 0)
    {
      if ((h = ULIB_AVL_TREE (insert) (&r->right, key)) <= 0)
	return h;

      switch (r->balance)
	{
	case 1:
	  t = r->right;
	  if (t->balance == 1)
	    {
	      r->right = t->left;
	      t->left = r;
	      r->balance = 0;
	    }
	  else
	    {
	      t = t->left;
	      r->right->left = t->right;
	      t->right = r->right;
	      r->right = t->left;
	      t->left = r;

	      if (t->balance == 1)
		t->left->balance = -1;
	      else
		t->left->balance = 0;

	      if (t->balance == -1)
		t->right->balance = 1;
	      else
		t->right->balance = 0;
	    }
	  t->balance = 0;
	  *pr = t;
	  return 0;

	case 0:
	  r->balance = 1;
	  return 1;

	case -1:
	  r->balance = 0;
	  return 0;
	}
    }

  return -1;
}


/* Balance the right subtree after a deletion on the left subtree
   decreases the height.  Return whether the height of the tree
   changed. */
static int
ULIB_AVL_TREE (balance_right) (ULIB_AVL_TREE_TYPE **pr)
{
  ULIB_AVL_TREE_TYPE *t, *r;

  r = *pr;
  switch (r->balance)
    {
    case -1:
      r->balance = 0;
      return 1;

    case 0:
      r->balance = 1;
      return 0;

    default:
    case 1:
      t = r->right;
      if (t->balance >= 0)
	{
	  r->right = t->left;
	  t->left = r;

	  *pr = t;
	  if (t->balance == 0)
	    {
	      t->balance = -1;
	      t->left->balance = 1;
	      return 0;
	    }
	  else
	    {
	      t->balance = 0;
	      t->left->balance = 0;
	      return 1;
	    }
	}
      else
	{
	  t = t->left;
	  r->right->left = t->right;
	  t->right = r->right;

	  r->right = t->left;
	  t->left = r;

	  if (t->balance == -1)
	    t->right->balance = 1;
	  else
	    t->right->balance = 0;

	  if (t->balance == 1)
	    t->left->balance = -1;
	  else
	    t->left->balance = 0;

	  t->balance = 0;
	}
      *pr = t;
      return 1;
    }
}

/* Balance the left subtree after a deletion on the right subtree
   decreases the height.  Return whether the height of the tree
   changed.  */
static int
ULIB_AVL_TREE (balance_left) (ULIB_AVL_TREE_TYPE **pr)
{
  ULIB_AVL_TREE_TYPE *t, *r;

  r = *pr;
  switch (r->balance)
    {
    case 1:
      r->balance = 0;
      return 1;

    case 0:
      r->balance = -1;
      return 0;

    default:
    case -1:
      t = r->left;
      if (t->balance <= 0)
	{
	  r->left = t->right;
	  t->right = r;

	  *pr = t;
	  if (t->balance == 0)
	    {
	      t->balance = 1;
	      t->right->balance = -1;
	      return 0;
	    }
	  else
	    {
	      t->balance = 0;
	      t->right->balance = 0;
	      return 1;
	    }
	}
      else
	{
	  t = t->right;
	  r->left->right = t->left;
	  t->left = r->left;

	  r->left = t->right;
	  t->right = r;

	  if (t->balance == 1)
	    t->left->balance = -1;
	  else
	    t->left->balance = 0;

	  if (t->balance == -1)
	    t->right->balance = 1;
	  else
	    t->right->balance = 0;

	  t->balance = 0;
	}
      *pr = t;
      return 1;
    }
}

/* Remove the node with the maximum key.  Return whether the height of
   the tree changed.  */
static int
ULIB_AVL_TREE (delete_max) (ULIB_AVL_TREE_TYPE **pr, ULIB_AVL_TREE_TYPE **rn)
{
  ULIB_AVL_TREE_TYPE *r;

  r = *pr;
  if (r->right)
    {
      if (ULIB_AVL_TREE (delete_max) (&r->right, rn))
	return ULIB_AVL_TREE (balance_left) (pr);
      else
	return 0;
    }
  else
    {
      *rn = r;
      *pr = r->left;
      return 1;
    }
}

/* Remove a key from an AVL tree.  Return negative if the key was not
   found or whether the height of the tree changed.  */
static int
ULIB_AVL_TREE (del) (ULIB_AVL_TREE_TYPE **pr,
		     ULIB_AVL_TREE_KEY_TYPE key, ULIB_AVL_TREE_TYPE **dn)
{
  ULIB_AVL_TREE_TYPE *t, *r;
  int h;

  r = *pr;
  if (r == 0)
    return -1;

  if (ULIB_AVL_TREE_COMPARE (r->key, key) > 0)
    {
      if ((h = ULIB_AVL_TREE (del) (&r->left, key, dn)) > 0)
	return ULIB_AVL_TREE (balance_right) (pr);
      else
	return h;
    }

  if (ULIB_AVL_TREE_COMPARE (r->key, key) < 0)
    {
      if ((h = ULIB_AVL_TREE (del) (&r->right, key, dn)) > 0)
	return ULIB_AVL_TREE (balance_left) (pr);
      else
	return h;
    }

  *dn = r;
  if (r->left && r->right)
    {
      h = ULIB_AVL_TREE (delete_max) (&r->left, &t);
      t->left = r->left;
      t->right = r->right;
      t->balance = r->balance;

      *pr = t;
      if (h)
	return ULIB_AVL_TREE (balance_right) (pr);
      else
	return 0;
    }

  *pr = (r->left == 0) ? r->right : r->left;
  return 1;
}


/* Lookup a key in the tree.  */
#ifdef ULIB_AVL_TREE_DATA_TYPE
ULIB_STATIC ULIB_AVL_TREE_DATA_TYPE *
#else
ULIB_STATIC int
#endif
ULIB_AVL_TREE (lookup) (const ULIB_AVL_TREE_TYPE *r,
			ULIB_AVL_TREE_KEY_TYPE key)
{
  while (r)
    {
      if (ULIB_AVL_TREE_COMPARE (key, r->key) < 0)
	r = r->left;
      else if (ULIB_AVL_TREE_COMPARE (key, r->key) > 0)
	r = r->right;
      else
	{
#ifdef ULIB_AVL_TREE_DATA_TYPE
	  return &r->data;
#else
	  return 1;
#endif
	}
    }

  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
