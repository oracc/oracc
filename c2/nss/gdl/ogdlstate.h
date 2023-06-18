#ifndef GDLSTATE_H_
#define GDLSTATE_H_

enum gs_bracket  { SB_NO , SB_YE };

enum gs_stream   { SS_SUX, SS_AKK, SS_LIN, SS_DOC, SS_S4, SS_S5, SS_S6, SS_S7, SS_S8 };

enum gs_mode     { SM_LOGOSYLL, SM_ALPHA, SM_MORPH };

enum gs_boolean  { SB_FALSE, SB_TRUE };

typedef enum gs_bracket gs_bracket;
typedef enum gs_stream gs_stream;
typedef enum gs_mode gs_mode;
typedef enum gs_boolean gs_boolean;

enum group_types { GT_GROUP, GT_ORDER, GT_LIGATURE, GT_SURRO };

#define GS_LANG_MAX	((1<<6)-1)
#define GS_SCRIPT_MAX	((1<<4)-1)

struct gdlstate
{
  gs_bracket damaged:1;       	/* # ⸢ ⸣	*/
  gs_bracket lost:1;          	/* [ ]    	*/
  gs_bracket maybe:1;         	/* ( )    	*/

  gs_bracket det:1;		/* { } 		*/
  gs_bracket supplied:1;      	/* < >    	*/
  gs_bracket excised:1;       	/* << >>  	*/
  gs_bracket implied:1;       	/* <( )>  	*/
  gs_bracket erased:1;        	/* ((  // 	*/
  gs_bracket cancelled:1;     	/* ((+ // 	*/
  gs_bracket superposed:1;    	/* // ))  	*/
  gs_bracket glolin:1;        	/* {{ ST_LIN (linguistic gloss) }} */
  gs_bracket glodoc:1;        	/* {{ ST_DOC (document-oriented gloss) }} */

  gs_bracket damaged_o:1;       	/* # ⸢ ⸣	*/
  gs_bracket lost_o:1;          	/* [ ]    	*/
  gs_bracket maybe_o:1;         	/* ( )    	*/

  gs_bracket det_o:1;		/* { } 		*/
  gs_bracket supplied_o:1;      	/* < >    	*/
  gs_bracket excised_o:1;       	/* << >>  	*/
  gs_bracket implied_o:1;       	/* <( )>  	*/
  gs_bracket erased_o:1;        	/* ((  // 	*/
  gs_bracket cancelled_o:1;     	/* ((+ // 	*/
  gs_bracket superposed_o:1;    	/* // ))  	*/
  gs_bracket glolin_o:1;        	/* {{ ST_LIN (linguistic gloss) }} */
  gs_bracket glodoc_o:1;        	/* {{ ST_DOC (document-oriented gloss) }} */

  gs_bracket damaged_c:1;       	/* # ⸢ ⸣	*/
  gs_bracket lost_c:1;          	/* [ ]    	*/
  gs_bracket maybe_c:1;         	/* ( )    	*/

  gs_bracket det_c:1;		/* { } 		*/
  gs_bracket supplied_c:1;      	/* < >    	*/
  gs_bracket excised_c:1;       	/* << >>  	*/
  gs_bracket implied_c:1;       	/* <( )>  	*/
  gs_bracket erased_c:1;        	/* ((  // 	*/
  gs_bracket cancelled_c:1;     	/* ((+ // 	*/
  gs_bracket superposed_c:1;    	/* // ))  	*/
  gs_bracket glolin_c:1;        	/* {{ ST_LIN (linguistic gloss) }} */
  gs_bracket glodoc_c:1;        	/* {{ ST_DOC (document-oriented gloss) }} */

  gs_boolean f_query:1;       	/* ? */
  gs_boolean f_bang:1;        	/* ! */
  gs_boolean f_star:1;        	/* * */
  gs_boolean f_hash:1;        	/* # */
  gs_boolean f_plus:1;        	/* + */
  gs_boolean f_uf1:1;         	/* user flag 1 */
  gs_boolean f_uf2:1;         	/* user flag 2 */
  gs_boolean f_uf3:1;         	/* user flag 3 */
  gs_boolean f_uf4:1;         	/* user flag 4 */

  gs_boolean d_sem_i:1;		/* grapheme is a semantic determinative, implicit */
  gs_boolean d_sem_e:1;		/* grapheme is a semantic determinative, implicit */
  gs_boolean d_phon:1; 		/* grapheme is a phonetic indicator */
  				/* There is no d_var because variant
			   	   glosses must be encoded in {{ ... }}
			   	   with variant stream set */
  gs_boolean g_losy:1;
  gs_boolean g_morph:1;
  gs_boolean g_alpha:1;

  gs_boolean g_logo:1;		/* logographic use of grapheme */
  gs_boolean g_caps:1;		/* grapheme is has undetermined reading */
  gs_boolean g_force:1;		/* force char ~ for g_logo and $ for g_caps 
				   was used in input */
};

extern void gdlstate_rawxml(FILE *fp, struct gdlstate *sp);


#endif/*GDLSTATE_H_*/
