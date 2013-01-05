#ifndef PROJECT_H_
#define PROJECT_H_

#include "lang.h"

/* This is the collection of language data used by a project; each of
   these langinst pointers points to an array whose length is the same
   as the highest allocated langcode.  This means that testing if a
   language has an entry in any of the arrays is just a matter of
   testing, e.g., file[<langcode>]
 */
struct projdata
{
  char name[_MAX_PATH];
  char dir[_MAX_PATH];
  struct langinst*file[c_count]; /* file-specific data within the project */
  struct langinst*proj[c_count]; /* project datasets for each language */
  struct langinst*glob[c_count]; /* global fall-back */
  struct currdata*data[c_count]; /* per-language currdata for rapid language
				    context switching*/
  int tried_qpn;		 /* did we already try PNs for this project? */
  struct Formset *propernam_forms;
  struct Formset *nonproper_forms;
  const char *sublangs;
};

/* This is the collection of places to search for different types
   of data; it is updated when project is set and when languages
   are switched */
struct currdata
{
  const char *lang;
  const char *signlist_name;
  struct Hash_table *values; /* validator for sign values */
  struct Hash_table *snames; /* validator for sign names */
  const char *wordlist_name;  
  Hash_table *words;
  Hash_table *wids;
  struct langinst *this;     /* langinst structure that currdata is caching */
  struct langinst *global;   /* global dictionary for this lang */
  struct langinst *proj_qpn;
  struct langinst *global_qpn;
  struct charset  *cset;
  const char *sublangs;
};

extern struct projdata *curr_project, null_project;
extern struct currdata *curr_data;
extern struct currdata *saved_data;

/* Each signlist loaded gets an entry here to avoid loading any twice */
extern Hash_table *known_signlists;

/* Each wordlist loaded gets an entry here to avoid loading any twice */
extern Hash_table *known_wordlists;

/* Each formlist loaded gets an entry here to avoid loading any twice */
extern Hash_table *known_formlists;

/* Each namelist loaded gets an entry here to avoid loading any twice */
extern Hash_table *known_namelists;

/* Each project in a run gets an entry in here pointing to its
   projdata */
extern Hash_table *known_projects;

/* Each language in a run gets an entry in here giving the integer
   value of the langcode */
extern Hash_table *known_langs;

#endif/*PROJECT_H_*/

