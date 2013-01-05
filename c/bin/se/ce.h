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

extern int xml_output;
extern void xmdinit(const char *project);
extern int xmdinit2(const char *project);
extern void xmdprinter(const char *f);
extern void xmdprinter2(const char *f);
extern int xmd_field_count(void);

extern const char *project, *state;

#endif /*_CE_H*/
