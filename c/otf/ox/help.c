#include <stdio.h>
#include <psd_base.h>
#include "ox.h"
void
help()
{
  printf("ox version %d.%02d                          written by Steve Tinney\n",
	 major_version,minor_version);
  printf("\nUsage:\n");
  printf("  ox [-cpv?] [-a FILE] [-[b,g] FILE] [-l FILE] [-r FILE] [-x FILE] [ARGS]\n\n");
  printf("  -c  check only, do not produce XML output\n");
  printf("  -f  force atf2xtf to produce XML output if it can\n");
  printf("  -v  validate as well as doing check\n");
  printf("  -?  print this help screen\n");
  printf("  -a  set ATF file name; ARGS used if not given\n");
  printf("  -o  set OTF file name; ARGS used if not given\n");
  printf("  -b  write list of bad texts to FILE\n");
  printf("  -g  write list of good texts to FILE\n");
  printf("  -l  write error log to FILE\n");
  printf("  -m  check lemmatization\n");
  printf("  -p  check P-ids and names against catalog\n");
  printf("  -r  print RNC schema to FILE and exit with status=0\n");
  printf("  -x  write XML to FILE; stdout used if not given\n");
  printf("\natf2xtf is free software released under the GPL\n"
	 "  (http://www.gnu.org/licences/gpl.html)\n");
  printf("\nATF and OTF are file formats used by Oracc; see\n");
  printf("\thttp://oracc.museum.upenn.edu/doc\n");
  exit(2);
}
