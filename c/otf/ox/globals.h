#ifndef GLOBALS_H_
#define GLOBALS_H_
#include <stdio.h>

#define MAX_LINE_ID_BUF 1023
#define MAX_IDBUF_LEN     32
extern unsigned char idbuf[];

extern char line_id_buf[];
extern unsigned char *text_n;
extern const char sparse_lem_fields[];

extern unsigned const char *curr_line_label;

struct xcl_context;
extern struct xcl_context *text_xc;

extern FILE *f_log;
extern FILE *f_xml;
extern FILE *f_forms;
extern FILE *f_graphemes;

extern int atf_cbd_err;
extern int cbd_err_line;
extern int cbd_lem_sigs;
extern int check_lem;
extern int fuzzy_aliasing;
extern int lnum;
extern int need_lemm;
extern int no_destfile;
extern int no_pi;
extern int odt_serial;
extern int show_toks;
extern int sparse_lem;
extern int verbose;
extern int with_textid;

extern char *odsods_fn;
extern char *odslem_fn;
extern char *odsatf_fn;
extern const char *fallback_project;
extern const char *textid;

extern int check_only;
extern int no_destfile;
extern int one_big_file;
extern int perform_lem;
extern int process_detchars;
extern int word_matrix;

#endif/*GLOBALS_H_*/
