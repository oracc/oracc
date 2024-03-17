#ifndef _CE_H
#define _CE_H

enum ce_files { ce_byid , ce_summary , ce_xmd , ce_bad };

struct ce_config
{
  const char *tag;
  enum ce_files f;
  const char *ext;
  const char **proxies;
};

extern int link_fields;
extern int ood_mode;
extern int xml_output;
extern void gdfinit(void);
extern void xmdinit(const char *project);
extern int xmdinit2(const char *project);
extern void xmdprinter(const char *f);
extern void xmdprinter2(const char *f);
extern int xmd_field_count(void);
extern void xmd_print_labels(void);

extern int count_entries(const char *tmp, const char *option);
extern void set_entries(const char **entries, const char *option);

extern const char *project, *state;

#endif /*_CE_H*/
