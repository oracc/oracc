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
extern FILE *f_insts;
extern FILE *f_graphemes;

extern int atf_cbd_err;
extern const char *arg_trans_default_lang;
extern int arg_use_unicode;
extern int cbd_err_line;
extern int cbd_lem_sigs;
extern int check_lem;
extern int cuneify_notices;
extern int fuzzy_aliasing;
extern int gdl_bootstrap;
extern int line_is_unit;
extern int lnum;
extern int need_lemm;
extern int no_destfile;
extern int no_gdl_xids;
extern int no_pi;
extern int odt_serial;
extern int report_all_label_errors;
extern int show_toks;
extern int sparse_lem;
extern int verbose;
extern int with_textid;

extern char *autolemfile;
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

extern int check_links;
extern int lem_autolem;
extern int lem_dynalem;
extern const char * lem_dynalem_tab;
extern const char *xcl_project;
extern const char *output_dir;
extern int check_only;
extern int validate;
extern int do_cdl;
extern int do_show_insts;
extern int do_signnames;
extern int dump_sigs;
extern int no_xcl_map;
extern int lem_forms_raw;
extern int named_ents;
extern int perform_dsa;
extern int perform_nsa;
extern int harvest_notices;
extern int warn_unlemmatized;
extern int perform_lem;
extern int force;
extern int force_atf_extension;
extern int force_otf_extension;
extern int check_pnames;
extern int bootstrap_mode;

extern FILE*f_atf;
extern FILE*f_autolem;
extern FILE*f_bad;
extern FILE *f_unlemm;

#endif/*GLOBALS_H_*/
