#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype128.h>

struct item
{
  char *s;
  char pq;
  int text;
  int unit;
  int word;
  char flag;
};
struct item *items = NULL;
int items_alloced = 0;
int items_used = 0;

#define MAXL 512
char line[MAXL];

/* full compare of items; used for sorting and for finding when the
   key has a nonzero word id */
int
cmp_full(const void *a,const void *b)
{
  const struct item *ap = a;
  const struct item *bp = b;
  int ret = strcmp(ap->s,bp->s);
  if (!ret)
    return 0;
  if (ap->pq != bp->pq)
    return ap->pq - bp->pq;
  if (ap->text != bp->text)
    return ap->text - bp->text;
  if (ap->unit != bp->unit)
    return ap->unit - bp->unit;
  if (ap->word != bp->word)
    return ap->word - bp->word;
  return 0;
}

/* text-only cmp used for finding when key has zero word-id 
 */
int
cmp_text(const void *a,const void *b)
{
  const struct item *ap = a;
  const struct item *bp = b;
  int ret = strcmp(ap->s,bp->s);
  /* fprintf(stderr,"%d: %s; %s\n", ret, ap->s, bp->s); */
  if (!ret)
    return 0;
  if (ap->pq != bp->pq)
    return ap->pq - bp->pq;
  /* fprintf(stderr,"%d :: %d\n", ap->text, bp->text); */
  return ap->text - bp->text;
}

void
compress()
{
  struct item *insert,*rover;
  insert = rover = items;

  /* find first non-keepable item */
  for (; rover - items < items_used; ++rover)
    if (!rover->flag)
      break;
    else
      ++insert;

  /* now insert points at the first non-keepable item */
  while (rover - items < items_used)
    if (rover->flag == 0)
      ++rover;
    else
      *insert++ = *rover++;

  items_used = insert - items;

  rover = items;
  for (rover = items; rover - items < items_used; ++rover)
    rover->flag = 0;
}

void
dump_items()
{
  int i;
  int words = items[0].word;
  for (i = 0; i < items_used; ++i)
    {
      printf("%c%06d",items[i].pq,items[i].text);
      if (words)
	printf(".%d.%d",items[i].unit,items[i].word);
      putchar('\n');
    }
}

struct item *
find_item(struct item *findme)
{
  struct item *ip;
  if (findme->word)
    ip = bsearch(findme,items,items_used,sizeof(struct item),cmp_full);
  else
    ip = bsearch(findme,items,items_used,sizeof(struct item),cmp_text);
  /* fprintf(stderr,"found=>%s\n",ip ? ip->s : NULL); */
  return ip;
}

void
scan_item(struct item *ip, char *line)
{
  ip->s = strdup(line);
  ip->pq = *line;
  ip->text = atoi(&line[1]);
  if (line[7] == '.')
    {
      char *lp = &line[8];
      ip->unit = atoi(&line[8]);
      while (*lp && isdigit(*lp))
	++lp;
      if (*lp && *lp == '.')
	ip->word = atoi(&lp[1]);
    }
}

void
union_op()
{
  int ch;
  int n = 0;
  static struct item i, *ip;

  while (EOF != (ch = fgetc(stdin)))
    {
      if ('\n' == ch)
	{
	  line[n] = '\0';
	  n = 0;
	  scan_item(&i,line);
	  if (NULL != (ip = find_item(&i)))
	    {
	      ip->flag = 1;
	      if (!i.word)
		while (ip - items < items_used 
		       && ip->pq == (ip+1)->pq 
		       && ip->text == (ip+1)->text)
		  ++ip,ip->flag = 1;
	    }
	  ch = fgetc(stdin);
	  ungetc(ch,stdin);
	  if (ch == '#' || ch == EOF)
	    break;
	}
      else
	line[n++] = ch;
    }
}

int
main(int argc, char **argv)
{
  struct item *ip;
  int len;
  char sorted = 0;

  while (fgets(line,MAXL,stdin) != NULL)
    {
      if ('#' == *line)
	{
	  if (!sorted)
	    {
	      sorted = 1;
	      qsort(items,items_used,sizeof(struct item),cmp_full);
	    }
	  switch (line[1])
	    {
	    case '&':
	      union_op();
	      compress();
	      break;
	    default:
	      fprintf(stderr,"pqunion: only #& is supported so far\n");
	      exit(-1);
	      break;
	    }
	}
      else
	{
	  if (items_used == items_alloced)
	    {
	      items_alloced += 8196;
	      items = realloc(items, items_alloced * sizeof(struct item));
	    }
	  len = strlen(line);
	  if (len < 7)
	    {
	      fprintf(stderr,"pqunion: %s: bad line\n", line);
	      exit(-1);
	    }
	  else
	    {
	      if (line[len-1] == '\n')
		line[--len] = '\0';
	      ip = &items[items_used++];
	      scan_item(ip,line);
	    }
	}
    }
  dump_items();
#if 0
  if (argc > 1)
    {
      FILE *cfp = fopen(argv[1],"a");
      if (cfp)
	{
	  fprintf(cfp,"count %d\n",items_used);
	  fclose(cfp);
	}
      else
	{
	  fprintf(stderr,"pqunion: open failed on '%s'\n",argv[1]);
	  return 1;
	}
    }
#endif
  return 0;
}
