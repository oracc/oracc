#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oraccsys.h>
#include <roco.h>

int fields_from_row1 = 0;
int trtd_output = 0;
int xml_output = 0;

const char *xmltag = NULL, *rowtag = NULL, *celtag = NULL;

int
main(int argc, char *const *argv)
{
  Roco *r = NULL;
  
  options(argc, argv, "c:C:fnr:R:tx:X?");

  r = roco_load("-", fields_from_row1, xmltag, rowtag, celtag);

  if (xml_output)
    roco_write_xml(stdout, r);
  else
    roco_write(stdout, r);
}

const char *prog = "rocox";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = "";

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'C':
      roco_colorder = arg;
      break;
    case 'c':
      celtag = arg;
      break;
    case 'f':
      fields_from_row1 = 1;
      break;
    case 'n':
      roco_newline = 1;
      break;
    case 'R':
      roco_format = arg;
      break;
    case 'r':
      rowtag = arg;
      break;
    case 't':
      xmltag = "-";
      rowtag = "tr";
      celtag = "td";
      xml_output = 1;
      break;
    case 'x':
      xmltag = arg;
      xml_output = 1;
      break;
    case 'X':
      roco_xmlify = 0;
      break;
    case '?':
      help();
      exit(1);
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void help (void)
{
}
