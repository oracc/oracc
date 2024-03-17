#ifndef WM_H_
#define WM_H_

#include "types.h"

struct wid
{
  Two_bytes  l;
  Two_bytes  w;
};

struct wm_kwic
{
  Four_bytes t;
  struct wid w;
};

struct wm_unit
{
  Four_bytes t;
  struct wid w;
  Two_bytes s;
};

struct wm_range
{
  Four_bytes t;
  struct wid start;
  struct wid end;
};

enum wm_format { KU_KWIC , KU_LINE , KU_UNIT, KU_NONE };

extern const char *wm_names[5];

#define init_wm_names()				\
  wm_names[KU_KWIC]="kwic";			\
  wm_names[KU_LINE]="line";			\
  wm_names[KU_UNIT]="unit";			\
  wm_names[KU_NONE]="none";			\
  wm_names[4]="NULL"

#define WM_Q_BIT (1<<30)
#define WM_X_BIT (1<<31)
#define WM_COMPLEMENT (~(WM_Q_BIT|WM_X_BIT))

extern const char *project_of(const char*id);
extern const char *skip_project(const char *);
extern char wm_prefix_char(Four_bytes);
extern Four_bytes wm_text_id(Four_bytes);
extern Four_bytes wm_q(Four_bytes);
extern Four_bytes wm_x(Four_bytes);
extern void wm_lineword2(struct wid*, const char *);
extern void wm_lineword(struct wm_unit*,struct wm_kwic*,const char *);
extern void wm_text(Four_bytes*, const char *);
extern void wm_unitlemm(struct wm_unit*,const char *);

#endif/*WM_H_*/
