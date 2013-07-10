#ifndef OX_H_
#define OX_H_

#define _MAX_PATH 1024

#include <stdio.h>

#include "run.h"

extern const char*atffile;
extern const char *badfile;
extern const char*cdtfile;
extern const char *formsfile;
extern FILE*f_forms;
extern const char *goodfile;
extern FILE*f_good;
extern const char *graphemesfile;
extern FILE*f_graphemes;
extern const char *lemmafile;
extern FILE*f_lemma;
extern const char*logfile;
extern FILE*f_log;
extern const char*norm1file;
extern const char *rncfile;
extern FILE*f_rnc;
extern const char *xmlfile;
extern FILE*f_xml;
extern const char *unlemmfile;

extern const char *automatic_styles, *styles_styles;

extern const char *xtf_rnc;
extern char line_id_buf[];
extern const unsigned char *curr_line_label;

extern int bootstrap_mode;
extern int cdfrnc_len;
extern int check_lem;
extern int check_links;
extern int check_only;
extern int check_pnames;
extern int compound_warnings; 
extern int do_cdl;
extern int do_cuneify;
extern int dollar_fifo;
extern int force;
extern int global_psa_context;
extern int harvest_notices;
extern int ignore_plus;
extern int lem_autolem;
extern int lem_do_wrapup;
extern int lem_extended;
extern int lem_forms_raw;
extern int lem_percent_threshold;
extern int lem_standalone;
extern int lem_status;
extern int lem_system;
extern int named_ents;
extern int need_lemm;
extern int ng_debug;
extern int no_pi;
extern int no_strict_dollar;
extern int no_xcl_map;
extern int non_xtf_output;
extern int one_big_file; /* per-file outputs not supported yet */
extern int perform_dsa;
extern int perform_lem;
extern int perform_nsa;
extern int pretty;
extern int process_detchars;
extern int psa_nents;
extern int psa_props;
extern int psu;
extern int reg_debug;
extern int rnc_start;
extern int shadow_lem;
extern int show_toks;
extern int sparse_lem;
extern int status;
extern int use_unicode;
extern int validate;
extern int verbose;
extern int warn_unlemmatized;
extern int with_noform;
extern int with_textid;
extern int v_flag;

extern int agroups;
extern int bilingual;
extern int lexical;
extern int math_mode;
extern int mylines;
extern int saa_mode;
extern int state_validator_deferred;

extern struct nsa_context*global_nsa_context;

extern int ox_options(int argc, char **argv);
extern void post_option_setup(struct run_context *run);
extern void process_inputs(struct run_context *run, int argc, char **argv);
extern void set_project(struct proj_context *p, const char *proj);

extern const char *automatic_styles;
extern const char *errmsg_fn;
extern const char *inputs_from_file;
extern char *input_file;
extern const char *master_styles;
extern const char *project;
extern char project_base[];
extern const char *project_dir;
extern const char *arg_project;
extern const char *styles_styles;
extern const char *system_project;

#include "list.h"
extern List *cdt_master_list, *cdt_meta_list, *cdt_styles_list;

extern void help(void);
extern void ox_init(void);
extern void ox_term(void);

#endif/*OX_H_*/
