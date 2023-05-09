#ifndef GDLSTATE_H_
#define GDLSTATE_H_

enum gs_bracket  { SB_NO , SB_OP, SB_ON, SB_CL };

enum gs_stream   { SS_SUX, SS_AKK, SS_LIN, SS_DOC, SS_S4, SS_S5, SS_S6, SS_S7, SS_S8 };

enum gs_mode     { SM_LOGOSYLL, SM_ALPHA, SM_MORPH };

enum gs_boolean  { SB_FALSE, SB_TRUE };

typedef enum gs_bracket gs_bracket;
typedef enum gs_stream gs_stream;
typedef enum gs_mode gs_mode;
typedef enum gs_boolean gs_boolean;

enum group_types { GT_GROUP, GT_ORDER, GT_LIGATURE, GT_SURRO };

#define GS_LANG_MAX	((1<<8)-1)
#define GS_SCRIPT_MAX	((1<<6)-1)

struct gdlstate
{
  gs_bracket damaged:2;       	/* # ⸢ ⸣	*/
  gs_bracket lost:2;          	/* [ ]    	*/
  gs_bracket maybe:2;         	/* ( )    	*/

  gs_bracket det:2;		/* { } 		*/
  gs_bracket supplied:2;      	/* < >    	*/
  gs_bracket excised:2;       	/* << >>  	*/
  gs_bracket implied:2;       	/* <( )>  	*/
  gs_bracket erased:2;        	/* ((  // 	*/
  gs_bracket cancelled:2;     	/* ((+ // 	*/
  gs_bracket superposed:2;    	/* // ))  	*/
  gs_bracket glolin:2;        	/* {{ ST_LIN (linguistic gloss) }} */
  gs_bracket glodoc:2;        	/* {{ ST_DOC (document-oriented gloss) }} */
  
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

  gs_boolean g_logo:1;		/* logographic use of grapheme */
  gs_boolean g_caps:1;		/* grapheme is has undetermined reading */
  gs_boolean g_force:1;		/* force char ~ for g_logo and $ for g_caps 
				   was used in input */

  gs_stream stream:4;        	/* lang or meta stream */
  uint lang:8;          	/* current language */
  uint script:6;        	/* current script */
  gs_mode mode:2;          	/* current transliteration mode--logosyllabic, alphabetic, morphology */
};

#endif/*GDLSTATE_H_*/
