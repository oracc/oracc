#ifndef HELP_H_
#define HELP_H_

#define help_str help_crt

extern void help_longest_opt(const char *s);
extern void help_crt(const char *s, int prenl);
extern void help_tab(void);
extern void help_title(const char *s);
extern void help_usage(const char *s);
extern void help_heading(const char *s);
extern const char *help_tabs(const char *o);
extern void help_option(const char *opt, const char *s);

#endif/*HELP_H_*/
