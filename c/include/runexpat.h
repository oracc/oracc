#ifndef _RUNEXPAT_H
#define _RUNEXPAT_H
#include "../expat.h"
enum isource { i_stdin , i_names , i_list };

extern const char *pi_file;
extern int pi_line;

#define  runexpat(from,list,sH,eH) runexpatNSuD(from,list,sH,eH,NULL,NULL)
#define  runexpatNS(from,list,sH,eH,ns) runexpatNSuD(from,list,sH,eH,ns,NULL)

extern const char *findAttr(const char **atts, const char *name);
extern void charData_discard(void);
extern XML_Char*charData_retrieve(void);

extern void runexpatNSuD(enum isource from, void *list,
			 XML_StartElementHandler startElement, 
			 XML_EndElementHandler endElement,
			 XML_Char *ns_sep_p, void *userData);

extern const char *get_xml_id(const char **atts);
#endif /*_RUNEXPAT_H*/
