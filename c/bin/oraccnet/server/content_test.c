#include "resolver.h"
#include "content.c"

extern int options(int, char**,const char*);
extern int optind;
const char *input = NULL, *project = NULL;
static struct content_opts *cop;

int
main(int argc, char **argv)
{
  cop = content_new_options();
  options(argc, argv, "c:ef:gh:Hi:p:st:uw");
  content(cop, input);
  return 0;
}

int verbose = 0;
const char *prog = "xfrag";
int major_version = 1;
int minor_version = 0;
const char *usage_string = " [-cCHUNK -hHILITE -fFRAG] [-HHTML -sSIGS -uUNWRAP -wWRAP] [-p PROJECT -iINPUT]";
int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'c': cop->chunk_id = arg; break;
    case 'e': cop->echo = 1; break;
    case 'f': cop->frag_id = arg; break;			     
    case 'g': cop->ga = 1; break;
    case 'h': cop->hilite_id = arg; break;
    case 'H': cop->html = 1; break;
    case 'i': input = arg; break;
    case 'p': project = arg; break;
    case 's': cop->sigs = 1; break;
    case 't': cop->title = arg; break;
    case 'u': cop->unwrap = 1; break;
    case 'w': cop->wrap = 1; break;
    default: return 1;
    }
  return 0;
}
void help() {}
