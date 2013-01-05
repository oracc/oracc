#ifndef PSD_DA94_H_
#define PSD_DA94_H_ 1

#include <psdtypes.h>

#define CONTROL_A	0x01
#define CONTROL_B	0x02
#define CONTROL_C	0x03
#define CONTROL_D	0x04
#define CONTROL_E	0x05
#define CONTROL_N 	0x0E
#define CONTROL_P 	0x10
#define CONTROL_R 	0x12
#define CONTROL_S 	0x13
#define CONTROL_X 	0x18

#define SUM_CHAR	CONTROL_B
#define LOG_CHAR	CONTROL_X
#define END_CHAR	CONTROL_N
#define AKK_CHAR	CONTROL_E
#define AKK_GLOSS_CHAR  CONTROL_C
#define SUM_GLOSS_CHAR	CONTROL_S

#define RAISED_QUERY	0xBD
#define RAISED_BANG	0xBE
#define DINGIR		0xDC
#define LHB		0xA9
#define RHB		0xAA
#define GNA		0xA7
#define IA		0xA5
#define HETH		0xA3
#define UCHETH		0xA4
#define SHIN		0xAD
#define UCSHIN		0xAE
#define RAISED_SHIN	0xEE
#define SADHE   	0xAB
#define UCSADHE		0xAC
#define RAISED_SADHE	0xEF
#define TETH		0xB1
#define UCTETH		0xB2
#define RAISED_TETH	0xF0
#define SIN		0xAF
#define UCSIN		0xB0
#define ALEF    	0xB5
#define SUB_x		0xBA
#define SUB_y		0xBB
#define SUB_0		0xC8
#define SUB_1		0xC9
#define SUB_2		0xCA
#define SUB_3		0xCB
#define SUB_4		0xCC
#define SUB_5		0xCD
#define SUB_6		0xCE
#define SUB_7		0xCF
#define SUB_8		0xD0
#define SUB_9		0xD1
#define TIMES_SIGN 	0xBC
#define PRIME		0xB6
#define PPRIME		0xB7
#define I_ACUTE		0x82
#define SUP_LSQB	0xF9
#define SUP_RSQB	0xFA

#define ACUTE_a 0x80
#define ACUTE_e 0x81
#define ACUTE_i 0x82
#define ACUTE_u 0x84
#define GRAVE_a 0x85
#define GRAVE_e 0x86
#define GRAVE_i 0x87
#define GRAVE_u 0x89
#define ACUTE_A 0x97
#define ACUTE_E 0x98
#define ACUTE_I 0x99
#define ACUTE_U 0x9A
#define GRAVE_A 0x9B
#define GRAVE_E 0x9C
#define GRAVE_I 0x9D
#define GRAVE_U 0x9E
#define CIRCUMFLEX_a 0x8E
#define CIRCUMFLEX_e 0x8F
#define CIRCUMFLEX_i 0x90
#define CIRCUMFLEX_u 0x92

/* the type of the Charinfo structure utilised in da_ctype.c */
typedef struct CHARINFO
{
	Uchar flipcase;		/* the value of the char's opposite case (often same) */
	Uchar ascii;		/* the ascii equivalent, stripped of diacritics */
	Uchar vbase;		/* the vowel to replace an accented char with */
	Bitfield chartype:4;	/* the type of the character */
	Bitfield fattr:4;	/* the font attribute for the character */
} Charinfo;

/** symbolic names for referring to contents of Charinfo */
enum chartypes 
{
	CNTRL,		/* control char */
	LOWER,		/* lowercase letter */
	UPPER,		/* uppercase letter */
	DIGIT,		/* number, or subscript x or y */
	DELIM,		/* delimiter between graphemes */
	VOW_2,		/* vowel with index 2 */
	VOW_3,		/* vowel with index 3 */
	PAREN,		/* a parenthesis permitted outside grapheme */
	I_PAR,		/* a parenthesis permitted inside grapheme */
	HYPHB,		/* hyphen with both half brackets */
	HYPHL,		/* hyphen with left half bracket */
	HYPHR,		/* hyphen with right half bracket */
	DETER,		/* determinative */
	GREEK,
	OTHER
};

enum fattrs
{
	NON,		/* no attribute, as with ^@ */
	FON,		/* Font on */
	FOF, 		/* Font off */
	ROM,		/* roman face (Sumerian and English etc.) */
	AKK,		/* Akkadian */
	SUP,		/* superscript */
	SUB,		/* subscript */
	SPB,		/* superscript subscript numbers, as in na4 */
	SPA,		/* superscript Akkadian */
	DUB,		/* doubled font */
	UPS,		/* upside down font */
	GRK,		/* greek font */
	DA1		/* other char in da1 */
};

#define CHARS_IN_FONT	256

/**The base array defined in da_ctype.c */
extern Charinfo da_ctype[];

/** ctype macros to use the arrays above */
#define DAC_ASC(c)   da_ctype[(c)].ascii
#define DAC_TYP(c)   da_ctype[(c)].chartype
#define DAC_FAT(c)   da_ctype[(c)].fattr
#define DAC_VOW(c)   da_ctype[(c)].vbase
#define DAC_ISALPHA(c) ( UPPER==DAC_TYP(c) || LOWER==DAC_TYP(c) \
      || VOW_2==DAC_TYP(c) || VOW_3==DAC_TYP(c))
#define DAC_ISLOWER(c) ( LOWER==DAC_TYP(DAC_ASC((c))))	/*test the ASCII equivs, not the chars themselves*/
#define DAC_ISUPPER(c) ( UPPER==DAC_TYP(DAC_ASC((c))))
#define DAC_ISDIGIT(c) ( DIGIT==DAC_TYP(c))
/**This definition of DAC_ISDELIM means that special care must be taken if
 * '\0' could occur after a grapheme. '\0' is deliberately not given DELIM
 * class, to prevent nasty run-on bugs when doing while (!DAC_ISDELIM(*s))
 * or the like. If this case may arise (if you're doing input by word, not
 * by line, for example) you need to use something like 
 * if (!*s || DAC_ISDELIM(*s)) to test for delimiters.
 */
#define DAC_ISDELIM(c) ( DELIM==DAC_TYP(c))
#define DAC_ISSQB(c)  ( I_PAR == DAC_TYP(c))
#define DAC_ISALNUM(c) ( DAC_ISALPHA(c) || DAC_ISDIGIT(c))
#define DAC_ISGRAPH(c) ( DAC_ISALNUM(c) || DAC_ISSQB(c))
#define DAC_ISVOWEL(c)\
        ( 'a' == DAC_ASC(c) || 'e' == DAC_ASC(c) \
      ||  'i' == DAC_ASC(c) || 'u' == DAC_ASC(c) \
      ||  'A' == DAC_ASC(c) || 'E' == DAC_ASC(c) \
      ||  'I' == DAC_ASC(c) || 'U' == DAC_ASC(c) \
      || VOW_2==DAC_TYP(c) || VOW_3==DAC_TYP(c))
#define DAC_ISACCENTED(c)	(VOW_2==DAC_TYP(c) || VOW_3==DAC_TYP(c))
#define DAC_ISVOW_2(c)	(VOW_2==DAC_TYP(c))
#define DAC_ISVOW_3(c)	(VOW_3==DAC_TYP(c))
#define DAC_ISFSWITCH(c) (FON == DAC_FAT(c) || FOF == DAC_FAT(c))
#define DAC_ISPAREN(c)	(PAREN == DAC_TYP(c) || I_PAR == DAC_TYP(c))
#define DAC_ISCNTRL(c)	(CNTRL == DAC_TYP(c))
#define DAC_ISCONTROL	DAC_ISCNTRL
#define DAC_SUPER(c)	(SUP == DAC_FAT((c)))
#define DAC_FLIPCASE(c)	da_ctype[(c)].flipcase

enum Graphcmp_opts_e { GCMP_NOFOLDCASE, GCMP_FOLDCASE, 
			 GCMP_OBEY_DELIMS, GCMP_IGNORE_DELIMS  };
typedef enum Graphcmp_opts_e Graphcmp_opts_e;
extern void	graphcmp_init (Graphcmp_opts_e case_action, Graphcmp_opts_e delim_action);
extern int 	graphcmp (register const Uchar * s1, register const Uchar * s2);
extern Boolean gcmp_separate_compounds;
extern Boolean gcmp_separate_delimiters;

#endif /* PSD_DA94_H_ */
