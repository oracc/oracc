#ifndef _CDF_H
#define _CDF_H
#define ucc(a)       (unsigned const char *)a
#define uc(a)        (unsigned char *)a
#define cc(a)        (const char *)a

#define xstrcat(a,b) strcat(((char*)(a)),((const char *)(b)))
#define xstrchr(a,b) strchr(((char*)(a)),b)
#define xstrcmp(a,b) strcmp(((const char *)(a)),((const char *)(b)))
#define xstrcpy(a,b) strcpy(((char *)(a)),((char*)(b)))
#define xstrncpy(a,b,c) strncpy(((char *)(a)),((char*)(b)),c)
#define xxstrlen(a)   strlen(((const char *)(a)))
#define xstrncmp(a,b,c) strncmp(((const char *)(a)),((const char *)(b)),((ssize_t)(c)))

enum block_levels { 
  XTF, TEXT , OBJECT, SURFACE, COLUMN, LINE, CELL, FIELD, AGROUP, WORD, GRAPHEME ,
  DIVISION , MILESTONE, H1, H2, H3,
  TR_TRANS, TR_SWITCH, TR_LABEL, TR_UNIT, TR_SPAN, TR_NOTE, TR_H1, TR_H2, TR_H3,
  bl_top
};

#define list_locator(fp) \
  fprintf(fp, \
	  "%s\t%d\t%s\t%s\t%s\t%s\t", \
	  file, lnum, textid, text_n, line_id_buf, curr_line_label)

#endif /*_CDF_H*/
