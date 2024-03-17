/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: property.c,v 0.3 1997/09/08 14:50:17 sjt Exp s $
*/
#include <psd_base.h>
#include "index.h"
#include "property.h"

extern Two_bytes curr_properties;

static void push_property (Two_bytes prop);
static Two_bytes pop_property (void);

void
begin_property (int prop)
{
  push_property (curr_properties);
  set_property (curr_properties, prop);
}

void
end_property ()
{
  curr_properties = pop_property ();
}

static Two_bytes*property_stack;
static int property_stack_len, property_stack_top;

static void
push_property (Two_bytes props)
{
  if (property_stack_len == property_stack_top)
    {
      property_stack_len += 16;
      property_stack = realloc (property_stack, 
				property_stack_len * sizeof(Two_bytes));
    }
  property_stack[property_stack_top++] = props;
}

static Two_bytes
pop_property ()
{
  return property_stack[--property_stack_top];
}
