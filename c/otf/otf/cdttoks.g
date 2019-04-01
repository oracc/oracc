%{
#include "cdt.h"
/* N.B.: do not include any inline tokens here as it breaks
   block_tester in cdt_reader */
%}
struct cdt_node;
%%
document,        odt_text,    odt_document,         cdt_white, cdt_list
title,           odt_meta,    odt_title,            cdt_para,  cdt_text
style,           odt_style,   cdt_style,            cdt_para,  cdt_style
section,         odt_text,    odt_section,          cdt_line,  cdt_list
figure,          odt_text,    cdt_figure,           cdt_line,  cdt_list
insert,          odt_text,    cdt_insert,           cdt_line,  cdt_list
h,               odt_text,    odt_h,                cdt_para,  cdt_text
p,               odt_text,    odt_p,                cdt_para,  cdt_text
caption,         odt_text,    cdt_caption,          cdt_para,  cdt_text
atf,             odt_foreign, foreign_atf,          cdt_end,   cdt_data
atffile,         odt_none,    cdt_atf_file,         cdt_line,  cdt_none
xmlfile,         odt_none,    cdt_xml_file,         cdt_line,  cdt_none
bibliography,    odt_foreign, foreign_bibliography, cdt_end,   cdt_data
catalog,         odt_foreign, foreign_catalog,      cdt_end,   cdt_data
cattable,        odt_foreign, foreign_catalog,      cdt_end,   cdt_data
include,         odt_none,    cdt_include,          cdt_line,  cdt_none
project,         odt_meta,    cdt_project_c,        cdt_line,  cdt_none
end,             odt_none,    cdt_end_tag,          cdt_line,  cdt_none
newpage,		 odt_text,	  cdt_newpage,			cdt_white, cdt_none
newoddpage,	     odt_text,	  cdt_newoddpage,		cdt_white, cdt_none
pageno,		     odt_text,	  cdt_pageno,			cdt_line, cdt_none
image,           odt_text,    cdt_image,            cdt_self,  cdt_none
figno,		     odt_text,	  cdt_figno,			cdt_line, cdt_none
