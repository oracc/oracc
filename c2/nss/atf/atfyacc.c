#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <memo.h>
#include <pool.h>
#include "atf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <memo.h>
#include <pool.h>
#include "cat.h"

struct catchunk *
atfyacc(void)
{
  curratffile = atffile;
  atfparse();
  return cat_head;
}
