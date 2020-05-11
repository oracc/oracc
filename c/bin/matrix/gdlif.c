#include <stdio.h>
#include "globals.h"
#include "lang.h"
#include "gdl.h"

void
gdlif_init()
{
  pool_init();
  tree_init();
  textid = idbuf;
  warning_init();
  global_lang = lang_switch(NULL,"sux",NULL,NULL,0);
   /*current_state = set_state(s_global,s_text);*/
  atf_init();
  gdl_init();
  use_unicode = 1;
}
void
gdlif_term()
{
  atf_term();
  gdl_term();
  pool_term();
  tree_term(1);
}
void
gdl_print(FILE*fp,const unsigned char *gdlinput)
{
  struct node *res = NULL;
  f_xml = fp;
  if (strchr(gdlinput,' '))
    {
      fprintf(stderr,"gdlif: gdl_print passed a string with spaces\n");
      return;
    }

  res = gdl((unsigned char*)gdlinput, GDL_FRAG_OK);
  if (res)
    {
      /*res is a line container; serialize the grandchildren*/
      struct node *c = res->children.nodes[0];
      int i;
      for (i = 0; i < c->children.lastused; ++i)
	serialize(c->children.nodes[i],0);
    }
  else
    {
      fprintf(stderr,"gdlif: conversion of '%s' failed\n",gdlinput);
      fprintf(fp,"%s",gdlinput);
    }
}
