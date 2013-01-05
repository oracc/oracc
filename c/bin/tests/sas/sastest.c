#include <stdio.h>
#include <stdlib.h>
#include "sas.h"

struct sas_info *sip;
int verbose;
void
try(const char *f,const char *cf,const char *gw,const char *pos)
{
  const unsigned char *form = (const unsigned char *)f,*aform;
  aform = sas_alias_form(sip,form,
			 (unsigned char *)cf,(unsigned char *)gw,(unsigned char *)pos);
  fprintf(stderr,"form %s => aform %s\n",form,aform);
}

int
main(int argc, char **argv)
{
  const char *fn = "aliases.asa";
  sip = sas_asa_load(fn);
  try("da",NULL,NULL,NULL);
  try("du",NULL,NULL,NULL);
  try("mu-un-du-a-asz",NULL,NULL,NULL);
  try("mu-{un}du-a{asz}",NULL,NULL,NULL);
  try("{mu}un-du{a}-asz",NULL,NULL,NULL);
  try("{mu}{un}du{a}{asz}",NULL,NULL,NULL);
  try("im-{mu}{un}-ne-du-u{a}{asz}-sze3",NULL,NULL,NULL);
  try("bi2-in-ga2-ra","jar","place","V");
  try("bi2-in-ga2-ra","gar","pile","V");
  try("mu-un-ga2","jar","place","V");
  try("mu-un-GA",NULL,NULL,NULL);
  try("ma-an-GA",NULL,NULL,NULL);
  try("e3-e-de3",NULL,NULL,NULL);
  try("e3-a",NULL,NULL,NULL);
  try("mu-un-ba",NULL,NULL,NULL);
  try("mu-na-ba-ra",NULL,NULL,NULL);
  try("mu-na-ba",NULL,NULL,NULL);
  try("mu-un-ba-ra",NULL,NULL,NULL);
  try("mu-be-ra",NULL,NULL,NULL);
  try("mi-be-ra",NULL,NULL,NULL);
  try("mu-be-e",NULL,NULL,NULL);
  try("mi-be-a",NULL,NULL,NULL);
  try("mu-bo-ra",NULL,NULL,NULL);
  try("mi-bo-ra",NULL,NULL,NULL);
  try("mu-bo-e",NULL,NULL,NULL);
  try("mi-bo-a",NULL,NULL,NULL);
  try("mu-po-ra",NULL,NULL,NULL);
  try("mi-po-ra",NULL,NULL,NULL);
  try("mu-po-e",NULL,NULL,NULL);
  try("mi-po-a",NULL,NULL,NULL);
  return 0;
}

int major_version = 1; int minor_version = 0;
const char *prog = "gdltest";
const char *usage_string = "";
void help() { ; }
void opts() { ; }
