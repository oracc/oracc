#include <oraccsys.h>
#include <signlist.h>
#include <sx.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

static Hash *usigns;
static const char *sx_unicode_rx_mangle(struct sl_signlist *sl, const char *g);

static int cmp_by_len(const void *a, const void *b)
{
  return strlen(*(char**)b) - strlen(*(char**)a);
}

void
sx_unicode(struct sl_signlist *sl)
{
  usigns = hash_create(1024);
  /* Index the signs that have unames -- those are encoded so we treat
     them as atoms even if they are compounds */
  int i;  
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_unicode *Up = (sl->signs[i]->xref ? &sl->signs[i]->xref->U : &sl->signs[i]->U);
      if (Up->uname)
	{
	  if (Up->uhex)
	    hash_add(usigns, sl->signs[i]->name, (ucp)Up->uhex);
	  else
	    mesg_verr(&sl->signs[i]->inst->mloc, "sign %s has uname but no uhex\n", sl->signs[i]->name);
	}
    }

  /* Create a pattern to use for PCRE2 matching */
  int nk;
  const char **k = hash_keys2(usigns, &nk);
  qsort(k, nk, sizeof(const char *), cmp_by_len);

  List *kl = list_create(LIST_SINGLE);
  for (i = 0; i < nk; ++i)
    {
      if (i)
	list_add(kl, "|");
      list_add(kl, (void*)sx_unicode_rx_mangle(sl, k[i]));
    }

  unsigned const char *pat = list_concat(kl);

#if 0
  fputs((ccp)pat, stderr);
  fputc('\n', stderr);
#endif

  int errornumber;
  PCRE2_SIZE erroroffset;
  pcre2_code*cpat = pcre2_compile((PCRE2_SPTR)pat, PCRE2_ZERO_TERMINATED,
				  0, &errornumber, &erroroffset, NULL);
  if (cpat)
    {
      int rc;
      pcre2_match_data *match_data;
      fprintf(stderr, "sx: pcre2_compile OK\n");
      match_data = pcre2_match_data_create_from_pattern(re, NULL);

      rc = pcre2_match(
		       cpat,                   /* the compiled pattern */
		       subject,              /* the subject string */
		       subject_length,       /* the length of the subject */
		       0,                    /* start at offset 0 in the subject */
		       0,                    /* default options */
		       match_data,           /* block for storing the result */
		       NULL);                /* use default match context */
      
      /* Matching failed: handle error cases */
      
      if (rc < 0)
	{
	  switch(rc)
	    {
	    case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
	      /*                                                                                                                           
	       * Handle other special cases if you like                                                                                       
	       */
	    default: printf("Matching error %d\n", rc); break;
	    }
	  pcre2_match_data_free(match_data);   /* Release memory used for the match */
	  pcre2_code_free(re);                 /* data and the compiled pattern. */
	  return 1;
	}
      
      /* Match succeded. Get a pointer to the output vector, where string offsets are                                                  
	 stored. */
      PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
      printf("\nMatch succeeded at offset %d\n", (int)ovector[0]);

      /*************************************************************************                                                       
       * We have found the first match within the subject string. If the output *                                                       
       * vector wasn't big enough, say so. Then output any substrings that were *                                                       
       * captured.                                                              *                                                       
       *************************************************************************/
      
      /* The output vector wasn't big enough. This should not happen, because we used                                                  
	 pcre2_match_data_create_from_pattern() above. */
      
      if (rc == 0)
	printf("ovector was not big enough for all the captured substrings\n");
      
      /* Show substrings stored in the output vector by number. Obviously, in a real                                                   
	 application you might want to do things other than print them. */
      
      for (i = 0; i < rc; i++)
	{
	  PCRE2_SPTR substring_start = subject + ovector[2*i];
	  size_t substring_length = ovector[2*i+1] - ovector[2*i];
	  fprintf(stderr, "%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
	}
    }
  else
    {
      fprintf(stderr, "sx: pcre2_compile failed\n");
      PCRE2_UCHAR buffer[256];
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
      fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,
	     buffer);
    }
  
  /* check the compounds by building ucode sequences for them:
   *   if there is one in the signlist already, check that the newly built ucode is the same
   *   else set the sign's U.ucode appropriately
   */
  struct sl_inst *ip;
  for (ip = list_first(sl->compounds); ip; ip = list_next(sl->compounds))
    {
      unsigned const char *name = ip->type == 's' ? ip->u.s->name : ip->u.f->name;
      struct sl_unicode *Up = ip->type == 's' ? &ip->u.s->U : &ip->u.f->U;
      if (!Up->uhex)
	{
	  /*struct sl_token *tp = hash_find(sl->htoken, name);*/
	  if (Up->useq)
	    fprintf(stderr, "sx_unicode: checking @useq %s for %s\n", Up->useq, name);
	  else
	    fprintf(stderr, "sx_unicode: building @useq for %s\n", name);
	}
    }

  for (i = 0; i < sl->nsigns; ++i)
    {
      if (!sl->signs[i]->U.utf8)
	{
	  if (sl->signs[i]->U.uhex)
	    sl->signs[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.uhex), sl->p);
	  else if (sl->signs[i]->U.useq)
	    sl->signs[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.useq), sl->p);
	}
    }

  for (i = 0; i < sl->nforms; ++i)
    {
      if (!sl->forms[i]->U.utf8)
	{
	  if (sl->forms[i]->U.uhex)
	    sl->forms[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.uhex), sl->p);
	  else if (sl->forms[i]->U.useq)
	    sl->forms[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.useq), sl->p);
	}
    }

}

/**Prepare a sign-name for use in pattern-matching:
 *
 *   - remove enclosing |...|
 *   - map . to , 
 *   - map + to ;
 *   - map ( and ) to < and >
 *   - adding a ',' at beginning and end
 *
 * The mangled string is added as a hash key to usigns--there's no
 * possibility of conflict because of the bracketing with ,..., so 'A'
 * is mangled to ',A,'.
 *
 */
static const char *
sx_unicode_rx_mangle(struct sl_signlist *sl, const char *g)
{
  char *res = NULL;
  if ('|' == *g)
    {
      res = (char*)pool_alloc(strlen(g)+1, sl->p);
      ++g;
    }
  else
    {
      res = (char*)pool_alloc(strlen(g)+3, sl->p);
    }
  const char *src = g;
  char *dst = res;
  *dst++ = ',';
  while (*src)
    {
      if ('.' == *src)
	{
	  *dst++ = ',';
	  ++src;
	}
      else if ('+' == *src)
	{
	  *dst++ = ';';
	  ++src;
	}
      else if ('|' == *src)
	++src;
      else if ('(' == *src)
	{
	  *dst++ = '<';
	  ++src;
	}
      else if (')' == *src)
	{
	  *dst++ = '>';
	  ++src;
	}
      else
	*dst++ = *src++;
    }
  *dst++ = ',';
  *dst = '\0';
  hash_add(usigns, (uccp)res, hash_find(usigns, (uccp)g));
  return res;
}

void
sx_unicode_table(FILE *f, struct sl_signlist *sl)
{
  const char **u = hash_keys(usigns);
  int i;
  for (i = 0; u[i]; ++i)
    fprintf(f, "%s\t%s\n", u[i], (char*)hash_find(usigns, (uccp)u[i]));
}
