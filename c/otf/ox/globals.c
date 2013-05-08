#include <stdlib.h>
#include "globals.h"

char line_id_buf[MAX_LINE_ID_BUF+1];
unsigned char *text_n = NULL;

unsigned const char *curr_line_label;

struct xcl_context *text_xc = NULL;

FILE *f_xml = NULL;
FILE *f_forms = NULL;
FILE *f_graphemes = NULL;

int cbd_lem_sigs = 0;
int check_lem = 0;
int fuzzy_aliasing = 0;
int need_lemm = 0;
int no_destfile = 0;
int no_pi = 0;
int odt_serial = 0;
int show_toks = 0;
int verbose = 0;
int word_matrix = 0;

unsigned char idbuf[MAX_IDBUF_LEN];

char *odsods_fn = NULL, *odslem_fn = NULL, *odsatf_fn = NULL;

const char *textid;
const char *fallback_project = NULL;
