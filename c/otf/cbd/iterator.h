#ifndef _ITERATOR_H
#define _ITERATOR_H
enum ifnc {
  if_alias , if_allow ,
  if_bases ,
  if_cbd ,
  if_dcfs ,
  if_entry , if_end_cbd , if_end_entry ,
  if_forms ,
  if_meta , 
  if_parts , if_phon ,
  if_root ,
  if_senses , if_stems , 
  if_top
};

struct ifnc_map
{
  enum ifnc ix;
  iterator_fnc fnc;
};
#endif/*_ITERATOR_H*/
