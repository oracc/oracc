#ifndef SOURCES_H_
#define SOURCES_H_

enum SRCS_FACES_E { SFE_NONE, SFE_FACE, SFE_OBV, SFE_REV, SFE_TOP, SFE_BOT, SFE_LFT, SFE_RT };
typedef enum SRCS_FACES_E Srcs_faces_e;
typedef enum SRCS_DIV_E Srcs_div_e;

struct SRCS_TABLOC
{
  const Uchar *face_text;
  const Uchar *col_text;
  const Uchar *line_text;
  Srcs_faces_e face;
  int column;
  int line_val;
  int line_sub_val;
  struct SRCS_TABLOC *range_end;
  Boolean is_range_begin;
  Boolean is_range_end;
};
typedef struct SRCS_TABLOC Srcs_tabloc;

extern void srcs_add_name (Uchar *name, Line*line);
extern void srcs_init (void);
extern void srcs_in_lines (void);
#endif
