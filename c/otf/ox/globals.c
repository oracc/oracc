#include <stdlib.h>
#include "globals.h"

char line_id_buf[MAX_LINE_ID_BUF+1];
unsigned char *text_n = NULL;

unsigned const char *curr_line_label;

struct xcl_context *text_xc = NULL;

FILE *f_autolem = NULL;
FILE *fp_forms = NULL;
FILE *f_graphemes = NULL;
FILE *f_insts = NULL;
FILE *f_xml = NULL;

const char *arg_trans_default_lang = NULL;
int arg_use_unicode = 0;
int atf_needs_xmd = 0;
int cbd_lem_sigs = 0;
int check_lem = 0;
int cuneify_notices = 0;
int do_cuneify = 1;
int dump_sigs = 0;
int fuzzy_aliasing = 0;
int gdl_bootstrap = 0;
int use_ilem_conv = 0;
int lem_props_strict = 0;
int lem_props_yes = 0;
int line_is_unit = 0;
int need_lemm = 0;
int no_destfile = 0;
int no_gdl_xids = 1;
int no_pi = 0;
int odt_serial = 0;
int report_all_label_errors = 0;
int sentences_used = 0;
int show_toks = 0;
int verbose = 0;
int word_matrix = 0;

unsigned char idbuf[MAX_IDBUF_LEN];

char *odsods_fn = NULL, *odslem_fn = NULL, *odsatf_fn = NULL;
char *autolemfile = NULL;
const char *errmsg_fn = NULL;
/*const char *prog;*/
/*const char *textid;*/
const char *fallback_project = NULL;

FILE *f_bad;
FILE *f_good;
FILE *f_unlemm;
const char *lem_dynalem_tab;
const char *xcl_project;
const char *output_dir;
int check_links;
int lem_autolem;
int lem_autolem_NN_only;
int lem_dynalem;
int check_only;
int validate;
int do_cdl;
int do_show_insts;
int no_xcl_map;
int lem_forms_raw;
int named_ents;
int perform_dsa;
int perform_nsa;
int harvest_notices;
int warn_unlemmatized;
int perform_lem;
int force;
int force_atf_extension = 0;
int force_otf_extension = 0;
int check_pnames;
int bootstrap_mode;

#if 0
/* Stub routines to make lib/oracc/warning.c happy; ox doesn't use this yet */
void
msglist_warning(const char *file, int ln, const char *str)
{
  ;
}

void
msglist_vwarning(const char *file, int ln, const char *str, va_list ap)
{
  ;
}
#endif
