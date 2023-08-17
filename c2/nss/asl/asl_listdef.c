#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <list.h>
#include <pool.h>
#include <mesg.h>
#include <signlist.h>

static int
npad(int e)
{
  if (e >= 10000)
    return 4;
  else if (e >= 1000)
    return 3;
  else if (e >= 100)
    return 2;
  else if (e >= 10)
    return 1;
  return 0;
}

/**return 0 on success, 1 on failure
 */
void
asl_bld_listdef(Mloc *locp, struct sl_signlist *sl, const char *name, const char *in)
{
  const char *str, *top;
  const char *save = (ccp)pool_copy((uccp)in, sl->p);
  char buf[32], *end, *ins, *ins_base;
  int status = 0, hexflag = 0;
  List *nlist = NULL;
  
  if (strlen(name) > 6)
    {
      mesg_verr(locp, "signlist name restricted to maximum of 6 characters: %s is too long\n", sl);
      return;
    }
  strcpy(buf, name);
  ins_base = ins = buf + strlen(name);
  top = &buf[31];
  nlist = list_create(LIST_SINGLE);
  for (str = in; str; ++str)
    {
      /* FIXME: This implmentation means that singleton items only
	 work properly if they are given after all ranges */
      if (strchr(str, '-'))
	{
	  int rb, re;
	  if ('0' == str[0] && 'x' == str[1])
	    {
	      rb = strtol(str, &end, 16);
	      hexflag = 1;
	    }
	  else
	    rb = strtol(str, &end, 10);
	  if (end && '-' == *end)
	    {
	      str = end + 1;
	      if ('0' == str[0] && 'x' == str[1])
		re = strtol(str, &end, 16);
	      else
		re = strtol(str, &end, 10);
	      if (end)
		{
		  int x, pow10 = 10;
		  int zeroes = npad(re);
		  char pad[zeroes+1];
		  for (x = 0; x < zeroes; ++x)
		    pad[x] = '0';
		  pad[x] = '\0';
		  for (x = rb; x <= re; ++x)
		    {
		      char *l = NULL;
		      int n;
		      if (x > pow10)
			{
			  /* for a range MZL 333-334 reset the padding
			     to account for the start value */
			  while (x > pow10)
			    {
			      pow10 *= 10;
			      pad[strlen(pad)-1] = '\0';
			    }
			}
		      else if (x == pow10)
			{
			  pow10 *= 10;
			  pad[strlen(pad)-1] = '\0';
			}
		      if (hexflag)
			{
			  n = snprintf(l, 0, "%s%X", name, x);
			  l = (char*)pool_alloc(n+1, sl->p);
			  snprintf(l, n+1, "%s%X", name, x);
			}
		      else
			{
			  n = snprintf(l, 0, "%s%s%d", name, pad, x);
			  l = (char*)pool_alloc(n+1, sl->p);
			  snprintf(l, n+1, "%s%s%d", name, pad, x);
			}
		      asl_bld_token(locp, sl, (ucp)l, 1);
		      list_add(nlist, l);
		    }
		  str = end;
		}
	      else
		{
		  mesg_verr(locp, "bad end in range %s\n", str);
		  ++status;
		  while (*str && !isspace(*str))
		    ++str;
		}
	    }
	  else
	    {
	      mesg_verr(locp, "bad start in range %s\n", str);
	      ++status;
	      while (*str && !isspace(*str))
		++str;
	    }	  
	}
      else
	{
	  ins = ins_base;
	  while (*str && !isspace(*str))
	    {
	      if (ins == top)
		{
		  *ins = '\0';
		  mesg_verr(locp, "signlist number %s is too long\n", buf);
		  return;
		}
	      else
		*ins++ = *str++;
	    }
	  *ins = '\0';
	  ucp l = pool_copy((uccp)buf, sl->p);
	  list_add(nlist, l);
	  asl_bld_token(locp, sl, (ucp)l, 1);
	  while (*str && isspace(str[1]))
	    ++str;
	}
      if ('\0' == *str)
	break;
    }
  if (!status)
    {
      if (!hash_find(sl->listdefs, (uccp)name))
	{
	  struct sl_listdef *ldp = calloc(1, sizeof(struct sl_listdef));
	  ldp->str = save;
	  ldp->name = pool_copy((uccp)name, sl->p);
	  ldp->nnames = list_len(nlist);
	  ldp->names = list2chars(nlist);
	  ldp->seen = hash_create(128);
	  ldp->inst.type = 'd';
	  ldp->inst.u.d = ldp;
	  sl->curr_inst = &ldp->inst;
	  
	  hash_add(sl->listdefs, ldp->name, ldp);
	}
      else
	{
	  mesg_verr(locp, "repeated @listdef %s\n", name);
	  ++status;
	}
    }
  if (nlist)
    list_free(nlist, NULL);
  return;
}

#if 0
/* does not work with revised interface for use in nss/asl */
int
main(int argc, char **argv)
{
  const char *in = "1-110 11a 11b";

  x("MZL", in);
}
#endif
