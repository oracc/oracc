#define NULL (void*)0
#include "types.h"
#include "selib.h"

/* some indexes need different rules for different fields, like the cbd */

struct expr_rules rulestab[] = 
  {
    { d_cat   , "~_"  , "-.,^=" , "+-3w" , "+-0r" , 
      KM_REDUCE|KM_FOLD|KM_2VOWEL , 
      KM_ATF2UTF|KM_REDUCE|KM_FOLD|KM_2VOWEL , 
      g_record, 'P', 'Q' },
    { d_cbd   , "-~_" , ";,"    , "+-1w" , "+-0r" , 
      KM_FOLD|KM_REDUCE /*|KM_2VOWEL*/ ,
      KM_ATF2UTF|KM_GRAPHEME|KM_REDUCE|KM_FOLD /*|KM_2VOWEL*/ ,
      g_record, 'x', 'x' },
    { d_tra   ,  "-~_" , ";,"   , "+-1w" , "+-0r" , 
      KM_REDUCE|KM_FOLD|KM_2VOWEL|KM_STEM , 
      KM_ATF2UTF|KM_REDUCE|KM_FOLD|KM_2VOWEL|KM_STEM , 
      g_word,   'v', 'v' },
    { d_lem   , "-~_.=" , NULL    , "+-1w" , "+0f"  , 
      KM_REDUCE /*|KM_FOLD|KM_2VOWEL*/ , 
      KM_ATF2UTF|KM_GRAPHEME|KM_REDUCE /*|KM_FOLD|KM_2VOWEL*/ , 
      g_word,   'P', 'Q' },
    { d_l24   , "-~_.=" , NULL    , "+-1w" , "+0f"  , 
      KM_REDUCE|KM_FOLD /*|KM_2VOWEL*/ , 
      KM_ATF2UTF|KM_GRAPHEME|KM_REDUCE|KM_FOLD /*|KM_2VOWEL*/ , 
      g_word,   'P', 'Q' },
    { d_txt   , "-~_.=" , NULL    , "+-1w" , "+0f"  , 
      KM_REDUCE /*|KM_FOLD|KM_2VOWEL*/ , 
      KM_ATF2UTF|KM_GRAPHEME|KM_REDUCE /*|KM_FOLD|KM_2VOWEL*/ , 
      g_word,   'P', 'Q' },
    { d_uni   , "-~_.=" , NULL    , "+-1w" , "+0f"  , 
      KM_REDUCE|KM_FOLD /*|KM_2VOWEL*/ , 
      KM_REDUCE|KM_FOLD /*|KM_2VOWEL*/ , 
      g_word,   'P', 'Q' },
    { d_NONE  , NULL  , NULL    , NULL   , NULL   , 
      0 , 
      0 , 
      g_record, 'x', 'x' },
  };

const char * const datum_type_names[] = 
  {
    "d_cat" , "d_cbd" , "d_tra" , "d_txt" , "d_uni" , "d_NONE",
  };
