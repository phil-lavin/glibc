/* Subroutines needed for unwinding stack frames for exception handling.  */
/* Copyright (C) 1997-2025 Free Software Foundation, Inc.

   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifdef _LIBC
# include <shlib-compat.h>
#endif

#if !defined _LIBC || SHLIB_COMPAT (libc, GLIBC_2_0, GLIBC_2_2_5)

#ifdef _LIBC
#include <stdlib.h>
#include <string.h>
#include <libc-lock.h>
#include <dwarf2.h>
#include <unwind.h>
#define NO_BASE_OF_ENCODED_VALUE
#include <unwind-pe.h>
#include <unwind-dw2-fde.h>
#else
#ifndef _Unwind_Find_FDE
#include "tconfig.h"
#include "tsystem.h"
#include "dwarf2.h"
#include "unwind.h"
#define NO_BASE_OF_ENCODED_VALUE
#include "unwind-pe.h"
#include "unwind-dw2-fde.h"
#include "gthr.h"
#endif
#endif

/* The unseen_objects list contains objects that have been registered
   but not yet categorized in any way.  The seen_objects list has had
   it's pc_begin and count fields initialized at minimum, and is sorted
   by decreasing value of pc_begin.  */
static struct object *unseen_objects;
static struct object *seen_objects;

#ifdef _LIBC

__libc_lock_define_initialized (static, object_mutex)
#define init_object_mutex_once()
#define __gthread_mutex_lock(m) __libc_lock_lock (*(m))
#define __gthread_mutex_unlock(m) __libc_lock_unlock (*(m))

void __register_frame_info_bases (void *begin, struct object *ob,
				  void *tbase, void *dbase);
hidden_proto (__register_frame_info_bases)
void __register_frame_info_table_bases (void *begin,
					struct object *ob,
					void *tbase, void *dbase);
hidden_proto (__register_frame_info_table_bases)
void *__deregister_frame_info_bases (void *begin);
hidden_proto (__deregister_frame_info_bases)

#else

#ifdef __GTHREAD_MUTEX_INIT
static __gthread_mutex_t object_mutex = __GTHREAD_MUTEX_INIT;
#else
static __gthread_mutex_t object_mutex;
#endif

#ifdef __GTHREAD_MUTEX_INIT_FUNCTION
static void
init_object_mutex (void)
{
  __GTHREAD_MUTEX_INIT_FUNCTION (&object_mutex);
}

static void
init_object_mutex_once (void)
{
  static __gthread_once_t once = __GTHREAD_ONCE_INIT;
  __gthread_once (&once, init_object_mutex);
}
#else
#define init_object_mutex_once()
#endif

#endif /* _LIBC */

/* Called from crtbegin.o to register the unwind info for an object.  */

void
__register_frame_info_bases (void *begin, struct object *ob,
			     void *tbase, void *dbase)
{
  /* If .eh_frame is empty, don't register at all.  */
  if (*(uword *) begin == 0)
    return;

  ob->pc_begin = (void *)-1;
  ob->tbase = tbase;
  ob->dbase = dbase;
  ob->u.single = begin;
  ob->s.i = 0;
  ob->s.b.encoding = DW_EH_PE_omit;
#ifdef DWARF2_OBJECT_END_PTR_EXTENSION
  ob->fde_end = NULL;
#endif

  init_object_mutex_once ();
  __gthread_mutex_lock (&object_mutex);

  ob->next = unseen_objects;
  unseen_objects = ob;

  __gthread_mutex_unlock (&object_mutex);
}
hidden_def (__register_frame_info_bases)

void
__register_frame_info (void *begin, struct object *ob)
{
  __register_frame_info_bases (begin, ob, 0, 0);
}

void
__register_frame (void *begin)
{
  struct object *ob;

  /* If .eh_frame is empty, don't register at all.  */
  if (*(uword *) begin == 0)
    return;

  ob = (struct object *) malloc (sizeof (struct object));
  __register_frame_info_bases (begin, ob, 0, 0);
}

/* Similar, but BEGIN is actually a pointer to a table of unwind entries
   for different translation units.  Called from the file generated by
   collect2.  */

void
__register_frame_info_table_bases (void *begin, struct object *ob,
				   void *tbase, void *dbase)
{
  ob->pc_begin = (void *)-1;
  ob->tbase = tbase;
  ob->dbase = dbase;
  ob->u.array = begin;
  ob->s.i = 0;
  ob->s.b.from_array = 1;
  ob->s.b.encoding = DW_EH_PE_omit;

  init_object_mutex_once ();
  __gthread_mutex_lock (&object_mutex);

  ob->next = unseen_objects;
  unseen_objects = ob;

  __gthread_mutex_unlock (&object_mutex);
}
hidden_def (__register_frame_info_table_bases)

void
__register_frame_info_table (void *begin, struct object *ob)
{
  __register_frame_info_table_bases (begin, ob, 0, 0);
}

void
__register_frame_table (void *begin)
{
  struct object *ob = (struct object *) malloc (sizeof (struct object));
  __register_frame_info_table_bases (begin, ob, 0, 0);
}

/* Called from crtbegin.o to deregister the unwind info for an object.  */
/* ??? Glibc has for a while now exported __register_frame_info and
   __deregister_frame_info.  If we call __register_frame_info_bases
   from crtbegin (wherein it is declared weak), and this object does
   not get pulled from libgcc.a for other reasons, then the
   invocation of __deregister_frame_info will be resolved from glibc.
   Since the registration did not happen there, we'll abort.

   Therefore, declare a new deregistration entry point that does the
   exact same thing, but will resolve to the same library as
   implements __register_frame_info_bases.  */

void *
__deregister_frame_info_bases (void *begin)
{
  struct object **p;
  struct object *ob = 0;
  struct fde_vector *tofree = NULL;

  /* If .eh_frame is empty, we haven't registered.  */
  if (*(uword *) begin == 0)
    return ob;

  init_object_mutex_once ();
  __gthread_mutex_lock (&object_mutex);

  for (p = &unseen_objects; *p ; p = &(*p)->next)
    if ((*p)->u.single == begin)
      {
	ob = *p;
	*p = ob->next;
	goto out;
      }

  for (p = &seen_objects; *p ; p = &(*p)->next)
    if ((*p)->s.b.sorted)
      {
	if ((*p)->u.sort->orig_data == begin)
	  {
	    ob = *p;
	    *p = ob->next;
	    tofree = ob->u.sort;
	    goto out;
	  }
      }
    else
      {
	if ((*p)->u.single == begin)
	  {
	    ob = *p;
	    *p = ob->next;
	    goto out;
	  }
      }

  __gthread_mutex_unlock (&object_mutex);
  abort ();

 out:
  __gthread_mutex_unlock (&object_mutex);
  free (tofree);
  return (void *) ob;
}
hidden_def (__deregister_frame_info_bases)

void *
__deregister_frame_info (void *begin)
{
  return __deregister_frame_info_bases (begin);
}

void
__deregister_frame (void *begin)
{
  /* If .eh_frame is empty, we haven't registered.  */
  if (*(uword *) begin != 0)
    free (__deregister_frame_info_bases (begin));
}


/* Like base_of_encoded_value, but take the base from a struct object
   instead of an _Unwind_Context.  */

static _Unwind_Ptr
base_from_object (unsigned char encoding, struct object *ob)
{
  if (encoding == DW_EH_PE_omit)
    return 0;

  switch (encoding & 0x70)
    {
    case DW_EH_PE_absptr:
    case DW_EH_PE_pcrel:
    case DW_EH_PE_aligned:
      return 0;

    case DW_EH_PE_textrel:
      return (_Unwind_Ptr) ob->tbase;
    case DW_EH_PE_datarel:
      return (_Unwind_Ptr) ob->dbase;
    }
  abort ();
}

/* Return the FDE pointer encoding from the CIE.  */
/* ??? This is a subset of extract_cie_info from unwind-dw2.c.  */

static int
get_cie_encoding (struct dwarf_cie *cie)
{
  const unsigned char *aug, *p;
  _Unwind_Ptr dummy;
  _Unwind_Word utmp;
  _Unwind_Sword stmp;

  aug = cie->augmentation;
  if (aug[0] != 'z')
    return DW_EH_PE_absptr;

  /* Skip the augmentation string.  */
  p = aug + strlen ((const char *) aug) + 1;
  p = read_uleb128 (p, &utmp);		/* Skip code alignment.  */
  p = read_sleb128 (p, &stmp);		/* Skip data alignment.  */
  p++;					/* Skip return address column.  */

  aug++;				/* Skip 'z' */
  p = read_uleb128 (p, &utmp);		/* Skip augmentation length.  */
  while (1)
    {
      /* This is what we're looking for.  */
      if (*aug == 'R')
	return *p;
      /* Personality encoding and pointer.  */
      else if (*aug == 'P')
	{
	  /* ??? Avoid dereferencing indirect pointers, since we're
	     faking the base address.  Gotta keep DW_EH_PE_aligned
	     intact, however.  */
	  p = read_encoded_value_with_base (*p & 0x7F, 0, p + 1, &dummy);
	}
      /* LSDA encoding.  */
      else if (*aug == 'L')
	p++;
      /* Otherwise end of string, or unknown augmentation.  */
      else
	return DW_EH_PE_absptr;
      aug++;
    }
}

static inline int
get_fde_encoding (struct dwarf_fde *f)
{
  return get_cie_encoding (get_cie (f));
}


/* Sorting an array of FDEs by address.
   (Ideally we would have the linker sort the FDEs so we don't have to do
   it at run time. But the linkers are not yet prepared for this.)  */

/* Return the Nth pc_begin value from FDE x.  */

static inline _Unwind_Ptr
get_pc_begin (fde *x, size_t n)
{
  _Unwind_Ptr p;
  memcpy (&p, x->pc_begin + n * sizeof (_Unwind_Ptr), sizeof (_Unwind_Ptr));
  return p;
}

/* Comparison routines.  Three variants of increasing complexity.  */

static int
fde_unencoded_compare (struct object *ob __attribute__((unused)),
		       fde *x, fde *y)
{
  _Unwind_Ptr x_ptr = get_pc_begin (x, 0);
  _Unwind_Ptr y_ptr = get_pc_begin (y, 0);

  if (x_ptr > y_ptr)
    return 1;
  if (x_ptr < y_ptr)
    return -1;
  return 0;
}

static int
fde_single_encoding_compare (struct object *ob, fde *x, fde *y)
{
  _Unwind_Ptr base, x_ptr, y_ptr;

  base = base_from_object (ob->s.b.encoding, ob);
  read_encoded_value_with_base (ob->s.b.encoding, base, x->pc_begin, &x_ptr);
  read_encoded_value_with_base (ob->s.b.encoding, base, y->pc_begin, &y_ptr);

  if (x_ptr > y_ptr)
    return 1;
  if (x_ptr < y_ptr)
    return -1;
  return 0;
}

static int
fde_mixed_encoding_compare (struct object *ob, fde *x, fde *y)
{
  int x_encoding, y_encoding;
  _Unwind_Ptr x_ptr, y_ptr;

  x_encoding = get_fde_encoding (x);
  read_encoded_value_with_base (x_encoding, base_from_object (x_encoding, ob),
				x->pc_begin, &x_ptr);

  y_encoding = get_fde_encoding (y);
  read_encoded_value_with_base (y_encoding, base_from_object (y_encoding, ob),
				y->pc_begin, &y_ptr);

  if (x_ptr > y_ptr)
    return 1;
  if (x_ptr < y_ptr)
    return -1;
  return 0;
}

typedef int (*fde_compare_t) (struct object *, fde *, fde *);


/* This is a special mix of insertion sort and heap sort, optimized for
   the data sets that actually occur. They look like
   101 102 103 127 128 105 108 110 190 111 115 119 125 160 126 129 130.
   I.e. a linearly increasing sequence (coming from functions in the text
   section), with additionally a few unordered elements (coming from functions
   in gnu_linkonce sections) whose values are higher than the values in the
   surrounding linear sequence (but not necessarily higher than the values
   at the end of the linear sequence!).
   The worst-case total run time is O(N) + O(n log (n)), where N is the
   total number of FDEs and n is the number of erratic ones.  */

struct fde_accumulator
{
  struct fde_vector *linear;
  struct fde_vector *erratic;
};

static int
start_fde_sort (struct fde_accumulator *accu, size_t count)
{
  size_t size;
  if (! count)
    return 0;

  size = sizeof (struct fde_vector) + sizeof (fde *) * count;
  if ((accu->linear = (struct fde_vector *) malloc (size)))
    {
      accu->linear->count = 0;
      if ((accu->erratic = (struct fde_vector *) malloc (size)))
	accu->erratic->count = 0;
      return 1;
    }
  else
    return 0;
}

static inline void
fde_insert (struct fde_accumulator *accu, fde *this_fde)
{
  if (accu->linear)
    accu->linear->array[accu->linear->count++] = this_fde;
}

/* Split LINEAR into a linear sequence with low values and an erratic
   sequence with high values, put the linear one (of longest possible
   length) into LINEAR and the erratic one into ERRATIC. This is O(N).

   Because the longest linear sequence we are trying to locate within the
   incoming LINEAR array can be interspersed with (high valued) erratic
   entries.  We construct a chain indicating the sequenced entries.
   To avoid having to allocate this chain, we overlay it onto the space of
   the ERRATIC array during construction.  A final pass iterates over the
   chain to determine what should be placed in the ERRATIC array, and
   what is the linear sequence.  This overlay is safe from aliasing.  */

static void
fde_split (struct object *ob, fde_compare_t fde_compare,
	   struct fde_vector *linear, struct fde_vector *erratic)
{
  static fde *marker;
  size_t count = linear->count;
  fde **chain_end = &marker;
  size_t i, j, k;

  /* This should optimize out, but it is wise to make sure this assumption
     is correct. Should these have different sizes, we cannot cast between
     them and the overlaying onto ERRATIC will not work.  */
  if (sizeof (fde *) != sizeof (fde **))
    abort ();

  for (i = 0; i < count; i++)
    {
      fde **probe;

      for (probe = chain_end;
	   probe != &marker && fde_compare (ob, linear->array[i], *probe) < 0;
	   probe = chain_end)
	{
	  chain_end = (fde **) erratic->array[probe - linear->array];
	  erratic->array[probe - linear->array] = NULL;
	}
      erratic->array[i] = (fde *) chain_end;
      chain_end = &linear->array[i];
    }

  /* Each entry in LINEAR which is part of the linear sequence we have
     discovered will correspond to a non-NULL entry in the chain we built in
     the ERRATIC array.  */
  for (i = j = k = 0; i < count; i++)
    if (erratic->array[i])
      linear->array[j++] = linear->array[i];
    else
      erratic->array[k++] = linear->array[i];
  linear->count = j;
  erratic->count = k;
}

/* This is O(n log(n)).  BSD/OS defines heapsort in stdlib.h, so we must
   use a name that does not conflict.  */

static void
frame_heapsort (struct object *ob, fde_compare_t fde_compare,
		struct fde_vector *erratic)
{
  /* For a description of this algorithm, see:
     Samuel P. Harbison, Guy L. Steele Jr.: C, a reference manual, 2nd ed.,
     p. 60-61.  */
  fde ** a = erratic->array;
  /* A portion of the array is called a "heap" if for all i>=0:
     If i and 2i+1 are valid indices, then a[i] >= a[2i+1].
     If i and 2i+2 are valid indices, then a[i] >= a[2i+2].  */
#define SWAP(x,y) do { fde * tmp = x; x = y; y = tmp; } while (0)
  size_t n = erratic->count;
  size_t m = n;
  size_t i;

  while (m > 0)
    {
      /* Invariant: a[m..n-1] is a heap.  */
      m--;
      for (i = m; 2*i+1 < n; )
	{
	  if (2*i+2 < n
	      && fde_compare (ob, a[2*i+2], a[2*i+1]) > 0
	      && fde_compare (ob, a[2*i+2], a[i]) > 0)
	    {
	      SWAP (a[i], a[2*i+2]);
	      i = 2*i+2;
	    }
	  else if (fde_compare (ob, a[2*i+1], a[i]) > 0)
	    {
	      SWAP (a[i], a[2*i+1]);
	      i = 2*i+1;
	    }
	  else
	    break;
	}
    }
  while (n > 1)
    {
      /* Invariant: a[0..n-1] is a heap.  */
      n--;
      SWAP (a[0], a[n]);
      for (i = 0; 2*i+1 < n; )
	{
	  if (2*i+2 < n
	      && fde_compare (ob, a[2*i+2], a[2*i+1]) > 0
	      && fde_compare (ob, a[2*i+2], a[i]) > 0)
	    {
	      SWAP (a[i], a[2*i+2]);
	      i = 2*i+2;
	    }
	  else if (fde_compare (ob, a[2*i+1], a[i]) > 0)
	    {
	      SWAP (a[i], a[2*i+1]);
	      i = 2*i+1;
	    }
	  else
	    break;
	}
    }
#undef SWAP
}

/* Merge V1 and V2, both sorted, and put the result into V1.  */
static void
fde_merge (struct object *ob, fde_compare_t fde_compare,
	   struct fde_vector *v1, struct fde_vector *v2)
{
  size_t i1, i2;
  fde * fde2;

  i2 = v2->count;
  if (i2 > 0)
    {
      i1 = v1->count;
      do
	{
	  i2--;
	  fde2 = v2->array[i2];
	  while (i1 > 0 && fde_compare (ob, v1->array[i1-1], fde2) > 0)
	    {
	      v1->array[i1+i2] = v1->array[i1-1];
	      i1--;
	    }
	  v1->array[i1+i2] = fde2;
	}
      while (i2 > 0);
      v1->count += v2->count;
    }
}

static void
end_fde_sort (struct object *ob, struct fde_accumulator *accu, size_t count)
{
  fde_compare_t fde_compare;

  if (accu->linear->count != count)
    abort ();

  if (ob->s.b.mixed_encoding)
    fde_compare = fde_mixed_encoding_compare;
  else if (ob->s.b.encoding == DW_EH_PE_absptr)
    fde_compare = fde_unencoded_compare;
  else
    fde_compare = fde_single_encoding_compare;

  if (accu->erratic)
    {
      fde_split (ob, fde_compare, accu->linear, accu->erratic);
      if (accu->linear->count + accu->erratic->count != count)
	abort ();
      frame_heapsort (ob, fde_compare, accu->erratic);
      fde_merge (ob, fde_compare, accu->linear, accu->erratic);
      free (accu->erratic);
    }
  else
    {
      /* We've not managed to malloc an erratic array,
	 so heap sort in the linear one.  */
      frame_heapsort (ob, fde_compare, accu->linear);
    }
}


/* Update encoding, mixed_encoding, and pc_begin for OB for the
   fde array beginning at THIS_FDE.  Return the number of fdes
   encountered along the way.  */

static size_t
classify_object_over_fdes (struct object *ob, fde *this_fde)
{
  struct dwarf_cie *last_cie = 0;
  size_t count = 0;
  int encoding = DW_EH_PE_absptr;
  _Unwind_Ptr base = 0;

  for (; ! last_fde (ob, this_fde); this_fde = next_fde (this_fde))
    {
      struct dwarf_cie *this_cie;
      _Unwind_Ptr mask, pc_begin;

      /* Skip CIEs.  */
      if (this_fde->CIE_delta == 0)
	continue;

      /* Determine the encoding for this FDE.  Note mixed encoded
	 objects for later.  */
      this_cie = get_cie (this_fde);
      if (this_cie != last_cie)
	{
	  last_cie = this_cie;
	  encoding = get_cie_encoding (this_cie);
	  base = base_from_object (encoding, ob);
	  if (ob->s.b.encoding == DW_EH_PE_omit)
	    ob->s.b.encoding = encoding;
	  else if (ob->s.b.encoding != encoding)
	    ob->s.b.mixed_encoding = 1;
	}

      read_encoded_value_with_base (encoding, base, this_fde->pc_begin,
				    &pc_begin);

      /* Take care to ignore link-once functions that were removed.
	 In these cases, the function address will be NULL, but if
	 the encoding is smaller than a pointer a true NULL may not
	 be representable.  Assume 0 in the representable bits is NULL.  */
      mask = size_of_encoded_value (encoding);
      if (mask < sizeof (void *))
	mask = (1L << (mask << 3)) - 1;
      else
	mask = -1;

      if ((pc_begin & mask) == 0)
	continue;

      count += 1;
      if ((void *) pc_begin < ob->pc_begin)
	ob->pc_begin = (void *) pc_begin;
    }

  return count;
}

static void
add_fdes (struct object *ob, struct fde_accumulator *accu, fde *this_fde)
{
  struct dwarf_cie *last_cie = 0;
  int encoding = ob->s.b.encoding;
  _Unwind_Ptr base = base_from_object (ob->s.b.encoding, ob);

  for (; ! last_fde (ob, this_fde); this_fde = next_fde (this_fde))
    {
      struct dwarf_cie *this_cie;

      /* Skip CIEs.  */
      if (this_fde->CIE_delta == 0)
	continue;

      if (ob->s.b.mixed_encoding)
	{
	  /* Determine the encoding for this FDE.  Note mixed encoded
	     objects for later.  */
	  this_cie = get_cie (this_fde);
	  if (this_cie != last_cie)
	    {
	      last_cie = this_cie;
	      encoding = get_cie_encoding (this_cie);
	      base = base_from_object (encoding, ob);
	    }
	}

      if (encoding == DW_EH_PE_absptr)
	{
	  if (get_pc_begin (this_fde, 0) == 0)
	    continue;
	}
      else
	{
	  _Unwind_Ptr pc_begin, mask;

	  read_encoded_value_with_base (encoding, base, this_fde->pc_begin,
					&pc_begin);

	  /* Take care to ignore link-once functions that were removed.
	     In these cases, the function address will be NULL, but if
	     the encoding is smaller than a pointer a true NULL may not
	     be representable.  Assume 0 in the representable bits is NULL.  */
	  mask = size_of_encoded_value (encoding);
	  if (mask < sizeof (void *))
	    mask = (1L << (mask << 3)) - 1;
	  else
	    mask = -1;

	  if ((pc_begin & mask) == 0)
	    continue;
	}

      fde_insert (accu, this_fde);
    }
}

/* Set up a sorted array of pointers to FDEs for a loaded object.  We
   count up the entries before allocating the array because it's likely to
   be faster.  We can be called multiple times, should we have failed to
   allocate a sorted fde array on a previous occasion.  */

static void
init_object (struct object* ob)
{
  struct fde_accumulator accu;
  size_t count;

  count = ob->s.b.count;
  if (count == 0)
    {
      if (ob->s.b.from_array)
	{
	  fde **p = ob->u.array;
	  for (count = 0; *p; ++p)
	    count += classify_object_over_fdes (ob, *p);
	}
      else
	count = classify_object_over_fdes (ob, ob->u.single);

      /* The count field we have in the main struct object is somewhat
	 limited, but should suffice for virtually all cases.  If the
	 counted value doesn't fit, re-write a zero.  The worst that
	 happens is that we re-count next time -- admittedly non-trivial
	 in that this implies some 2M fdes, but at least we function.  */
      ob->s.b.count = count;
      if (ob->s.b.count != count)
	ob->s.b.count = 0;
    }

  if (!start_fde_sort (&accu, count))
    return;

  if (ob->s.b.from_array)
    {
      fde **p;
      for (p = ob->u.array; *p; ++p)
	add_fdes (ob, &accu, *p);
    }
  else
    add_fdes (ob, &accu, ob->u.single);

  end_fde_sort (ob, &accu, count);

  /* Save the original fde pointer, since this is the key by which the
     DSO will deregister the object.  */
  accu.linear->orig_data = ob->u.single;
  ob->u.sort = accu.linear;

  ob->s.b.sorted = 1;
}

/* A linear search through a set of FDEs for the given PC.  This is
   used when there was insufficient memory to allocate and sort an
   array.  */

static fde *
linear_search_fdes (struct object *ob, fde *this_fde, void *pc)
{
  struct dwarf_cie *last_cie = 0;
  int encoding = ob->s.b.encoding;
  _Unwind_Ptr base = base_from_object (ob->s.b.encoding, ob);

  for (; ! last_fde (ob, this_fde); this_fde = next_fde (this_fde))
    {
      struct dwarf_cie *this_cie;
      _Unwind_Ptr pc_begin, pc_range;

      /* Skip CIEs.  */
      if (this_fde->CIE_delta == 0)
	continue;

      if (ob->s.b.mixed_encoding)
	{
	  /* Determine the encoding for this FDE.  Note mixed encoded
	     objects for later.  */
	  this_cie = get_cie (this_fde);
	  if (this_cie != last_cie)
	    {
	      last_cie = this_cie;
	      encoding = get_cie_encoding (this_cie);
	      base = base_from_object (encoding, ob);
	    }
	}

      if (encoding == DW_EH_PE_absptr)
	{
	  pc_begin = get_pc_begin (this_fde, 0);
	  pc_range = get_pc_begin (this_fde, 1);
	  if (pc_begin == 0)
	    continue;
	}
      else
	{
	  _Unwind_Ptr mask;
	  const unsigned char *p;

	  p = read_encoded_value_with_base (encoding, base,
					    this_fde->pc_begin, &pc_begin);
	  read_encoded_value_with_base (encoding & 0x0F, 0, p, &pc_range);

	  /* Take care to ignore link-once functions that were removed.
	     In these cases, the function address will be NULL, but if
	     the encoding is smaller than a pointer a true NULL may not
	     be representable.  Assume 0 in the representable bits is NULL.  */
	  mask = size_of_encoded_value (encoding);
	  if (mask < sizeof (void *))
	    mask = (1L << (mask << 3)) - 1;
	  else
	    mask = -1;

	  if ((pc_begin & mask) == 0)
	    continue;
	}

      if ((_Unwind_Ptr) pc - pc_begin < pc_range)
	return this_fde;
    }

  return NULL;
}

/* Binary search for an FDE containing the given PC.  Here are three
   implementations of increasing complexity.  */

static fde *
binary_search_unencoded_fdes (struct object *ob, void *pc)
{
  struct fde_vector *vec = ob->u.sort;
  size_t lo, hi;

  for (lo = 0, hi = vec->count; lo < hi; )
    {
      size_t i = (lo + hi) / 2;
      fde *f = vec->array[i];
      void *pc_begin;
      uaddr pc_range;

      pc_begin = (void *) get_pc_begin (f, 0);
      pc_range = (uaddr) get_pc_begin (f, 1);

      if (pc < pc_begin)
	hi = i;
      else if (pc >= pc_begin + pc_range)
	lo = i + 1;
      else
	return f;
    }

  return NULL;
}

static fde *
binary_search_single_encoding_fdes (struct object *ob, void *pc)
{
  struct fde_vector *vec = ob->u.sort;
  int encoding = ob->s.b.encoding;
  _Unwind_Ptr base = base_from_object (encoding, ob);
  size_t lo, hi;

  for (lo = 0, hi = vec->count; lo < hi; )
    {
      size_t i = (lo + hi) / 2;
      fde *f = vec->array[i];
      _Unwind_Ptr pc_begin, pc_range;
      const unsigned char *p;

      p = read_encoded_value_with_base (encoding, base, f->pc_begin,
					&pc_begin);
      read_encoded_value_with_base (encoding & 0x0F, 0, p, &pc_range);

      if ((_Unwind_Ptr) pc < pc_begin)
	hi = i;
      else if ((_Unwind_Ptr) pc >= pc_begin + pc_range)
	lo = i + 1;
      else
	return f;
    }

  return NULL;
}

static fde *
binary_search_mixed_encoding_fdes (struct object *ob, void *pc)
{
  struct fde_vector *vec = ob->u.sort;
  size_t lo, hi;

  for (lo = 0, hi = vec->count; lo < hi; )
    {
      size_t i = (lo + hi) / 2;
      fde *f = vec->array[i];
      _Unwind_Ptr pc_begin, pc_range;
      const unsigned char *p;
      int encoding;

      encoding = get_fde_encoding (f);
      p = read_encoded_value_with_base (encoding,
					base_from_object (encoding, ob),
					f->pc_begin, &pc_begin);
      read_encoded_value_with_base (encoding & 0x0F, 0, p, &pc_range);

      if ((_Unwind_Ptr) pc < pc_begin)
	hi = i;
      else if ((_Unwind_Ptr) pc >= pc_begin + pc_range)
	lo = i + 1;
      else
	return f;
    }

  return NULL;
}

static fde *
search_object (struct object* ob, void *pc)
{
  /* If the data hasn't been sorted, try to do this now.  We may have
     more memory available than last time we tried.  */
  if (! ob->s.b.sorted)
    {
      init_object (ob);

      /* Despite the above comment, the normal reason to get here is
	 that we've not processed this object before.  A quick range
	 check is in order.  */
      if (pc < ob->pc_begin)
	return NULL;
    }

  if (ob->s.b.sorted)
    {
      if (ob->s.b.mixed_encoding)
	return binary_search_mixed_encoding_fdes (ob, pc);
      else if (ob->s.b.encoding == DW_EH_PE_absptr)
	return binary_search_unencoded_fdes (ob, pc);
      else
	return binary_search_single_encoding_fdes (ob, pc);
    }
  else
    {
      /* Long slow labourious linear search, cos we've no memory.  */
      if (ob->s.b.from_array)
	{
	  fde **p;
	  for (p = ob->u.array; *p ; p++)
	    {
	      fde *f = linear_search_fdes (ob, *p, pc);
	      if (f)
		return f;
	    }
	  return NULL;
	}
      else
	return linear_search_fdes (ob, ob->u.single, pc);
    }
}

fde *
_Unwind_Find_FDE (void *pc, struct dwarf_eh_bases *bases)
{
  struct object *ob;
  fde *f = NULL;

  init_object_mutex_once ();
  __gthread_mutex_lock (&object_mutex);

  /* Linear search through the classified objects, to find the one
     containing the pc.  Note that pc_begin is sorted descending, and
     we expect objects to be non-overlapping.  */
  for (ob = seen_objects; ob; ob = ob->next)
    if (pc >= ob->pc_begin)
      {
	f = search_object (ob, pc);
	if (f)
	  goto fini;
	break;
      }

  /* Classify and search the objects we've not yet processed.  */
  while ((ob = unseen_objects))
    {
      struct object **p;

      unseen_objects = ob->next;
      f = search_object (ob, pc);

      /* Insert the object into the classified list.  */
      for (p = &seen_objects; *p ; p = &(*p)->next)
	if ((*p)->pc_begin < ob->pc_begin)
	  break;
      ob->next = *p;
      *p = ob;

      if (f)
	goto fini;
    }

 fini:
  __gthread_mutex_unlock (&object_mutex);

  if (f)
    {
      int encoding;
      _Unwind_Ptr func;

      bases->tbase = ob->tbase;
      bases->dbase = ob->dbase;

      encoding = ob->s.b.encoding;
      if (ob->s.b.mixed_encoding)
	encoding = get_fde_encoding (f);
      read_encoded_value_with_base (encoding, base_from_object (encoding, ob),
				    f->pc_begin, &func);
      bases->func = (void *) func;
    }

  return f;
}

#endif
