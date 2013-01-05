#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "wm.h"

const char *
project_of(const char*id)
{
  static char project_buf[128];
  char *colon = strchr(id, ':');
  if (colon - id < 128)
    {
      strncpy(project_buf, id, colon-id);
      project_buf[colon-id] = '\0';
    }
  else
    {
      fprintf(stderr, "wm: project too long (%s)", id);
      exit(1);
    }
  return project_buf;
}

const char *
skip_project(const char*id)
{
  char *colon = strchr(id, ':');
  return colon ? (colon+1) : id;
}

char
wm_prefix_char(Four_bytes id)
{
  if (id & WM_Q_BIT)
    return 'Q';
  else if (id & WM_X_BIT)
    return 'X';
  else
    return 'P';
}
Four_bytes
wm_text_id(Four_bytes id)
{
  return id & WM_COMPLEMENT;
}
Four_bytes
wm_q(Four_bytes id)
{
  return id|WM_Q_BIT;
}
Four_bytes
wm_x(Four_bytes id)
{
  return id|WM_X_BIT;
}

void
wm_text(Four_bytes *tp, const char *id)
{
  Four_bytes t;
  id = skip_project(id);
  t = atoi(id+1);
  if (*id == 'Q')
    t = wm_q(t);
  else if (*id == 'X')
    t = wm_x(t);
  *tp = t;
}
void
wm_lineword2(struct wid *w, const char *id)
{
  id = skip_project(id);
  id += 8;
  w->l = atoi(id);
  while (*id && '.' != *id)
    ++id;
  ++id;
  w->w = atoi(id);
}
void
wm_lineword(struct wm_unit *unit, struct wm_kwic *kwic, const char *id)
{
  id = skip_project(id);
  wm_text(&unit->t, id);
  wm_lineword2(&unit->w, id);
  if (kwic)
    {
      kwic->t = unit->t;
      kwic->w = unit->w;
    }
}
void
wm_unitlemm(struct wm_unit *unit, const char *id)
{
  if (id)
    {
      id = skip_project(id);
      id += 9; /*P123456.U*/
      unit->s = atoi(id);
    }
}
