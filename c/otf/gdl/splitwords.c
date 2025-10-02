#include <stdlib.h>
#include <string.h>
#include <tree.h>

struct sw_node
{
  struct node *wp;
  struct sw_node *next;
} *sw_head = NULL, *sw_curr;

struct node *
sw_get_head(void)
{
  return sw_head->wp;
}

void
sw_add(struct node *wp)
{
  if (sw_head == NULL)
    {
      sw_curr = sw_head = malloc(sizeof(struct sw_node));
      sw_head->wp = wp;
      sw_head->next = NULL;
    }
  else
    {
      sw_curr->next = malloc(sizeof(struct sw_node));
      sw_curr = sw_curr->next;
      sw_curr->wp = wp;
      sw_curr->next = NULL;
    }
}

void
sw_free(void)
{
  struct sw_node *tmp;
  for (sw_curr = sw_head; sw_curr; sw_curr = tmp)
    {
      tmp = sw_curr->next;
      free(sw_curr);
    }
  sw_head = sw_curr = NULL;
}

char *
sw_refs(void)
{
  int alloced = 0;
  char *ret = NULL;
  for (sw_curr = sw_head; sw_curr; sw_curr = sw_curr->next)
    {
      const char *ref = (const char *)getAttr(sw_curr->wp,"xml:id");
      int space = ret ? 1 : 0;
      if (!ret || (strlen(ret)+strlen(ref) >= alloced))
	{
	  int new_alloc = alloced ? (alloced<<1) : 128;
	  int ret_len = ret ? strlen(ret) : 0;
	  while (ret_len + strlen(ref)+1 >= new_alloc)
	    new_alloc <<= 1;
	  ret = realloc(ret, alloced = new_alloc);
	  if (!ret_len)
	    *ret = '\0';
	}
      if (space)
	strcat(ret, " ");
      strcat(ret, ref);
    }
  return ret;
}

static char *
sw_last_open_curly(char *r)
{
  char *start = r;
  r += strlen(r);
  while (r > start)
    {
      if (r[-1] == '{')
	return --r;
      else
	--r;
    }
  return NULL;
}

char *
sw_form(void)
{
  int alloced = 0;
  char *ret = NULL;
  for (sw_curr = sw_head; sw_curr; sw_curr = sw_curr->next)
    {
      const char *frm = (const char *)getAttr(sw_curr->wp,"form");
      int hyphen = ret ? 1 : 0;
      if (!ret || (strlen(ret)+strlen(frm)+1 >= alloced))
	{
	  int new_alloc = alloced ? (alloced<<1) : 128;
	  int ret_len = ret ? strlen(ret) : 0;
	  while (ret_len + strlen(frm) >= new_alloc)
	    new_alloc <<= 1;
	  ret = realloc(ret, alloced = new_alloc);
	  if (!ret_len)
	    *ret = '\0';
	}
      if  (*frm == 0x1)
	{
	  memmove(ret+strlen(ret),frm+1,strlen(frm)-1);
	}
      else
	{
	  if (hyphen)
	    {
	      if (ret[strlen(ret)-1] != '}')
		strcat(ret,"-");
	      else
		{
		  char *prev_curly = sw_last_open_curly(ret);
		  if (prev_curly)
		    {
		      if (prev_curly > ret)
			{
			  if (!strchr("-.:", prev_curly[-1]))
			    strcat(ret, "-");
			}
		      /* else it must be a pre-det so no hyphen */
		    }
		  else
		    fprintf(stderr, "ox: internal error: no previous '{' in split word\n");
		}
	    }
	  strcat(ret, frm);
	}
    }
  return ret;
}
