#ifndef PSUS_H_
#define PSUS_H_ 1
struct sigset;
struct lang_context;
#include "xcl.h"
extern struct NL_context *psus2_init(struct sigset *sp);
extern void psus2(struct xcl_context *xc);
extern void psus2_term(void);
extern struct f2* psu2_register(const char *file, size_t lnum, 
				struct sigset *sp, const char *lang,
				const unsigned char *ngram_line,
				List *component_sigs);

#if 0
extern void ez_psu_store(unsigned char *line, struct lang *lang);
extern void ez_psu_process_save_list(struct xcl_context *xc, const char *lang);
#endif

#endif/*PSUS_H_*/
