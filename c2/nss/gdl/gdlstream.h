#ifndef GDLSTREAM_H_
#define GDLSTREAM_H_

/* Every word in GDL is part of a stream with some streams being
 * predefined and others able to be defined by users.
 *
 * The basic stream is S1, and is the stream for the language defined
 * in #atf: lang LANGUAGE. If there is not #atf: lang this is Sumerian.
 *
 * User definitions of streams have the form:
 *
 * #atf: stream SN <token>
 *
 * or
 *
 * #atf: stream SN-N <token>
 *
 * Where <token> is a short, one-word, descriptive token; for SN
 * <token> will usually be a language code.
 *
 * A language shift does not necessarily change the stream: if the
 * language is a logogram language (e.g., %s or %a in Hittite context)
 * the stream does not change because the logograms are part of the
 * same language context.
 *
 * In #atf: use bilingual the second language is S1; use bilingual
 * should support giving a sequence of languages which will each be a
 * stream in order to handle multilingual texts. S2 .. S5 are reserved
 * for additional multilingual streams, e.g., sum:akk:xhu lex texts
 *
 * In #atf: use lexical each field is its own stream, with S0 and S1
 * being the primary term and primary translation fields.
 *
 * When {( ... )} and {{ ... }} are used there is always a stream
 * switch, either to a predefined stream or a user-defined one.
 *
 * {{ ... }}: if a language-switch comes after {{ switch to the
 * 		language stream, often S1.
 *            if no language switch the gloss is taken as a variant
 *		gloss, S6
 *	      if /{{[0-9]+:/ switch to the numbered stream; S0 is
 *		invalid in this context.
 *
 * {( ... )}: switch to SN-8 = stream-oriented gloss (requires
 * language shift) or S0-9 = document-oriented gloss.
 *
 * Predefined streams: NEW::this is stream_lang
 *
 *  S0 language-neutral (cuneiform writing system)
 *  S1 language1
 *  S2 language2
 *  S3 language3
 *  S4 language4
 *  S5 language5
 *  S6 user-language1
 *  S7 user-language1
 *  S8 user-language1
 *  S9 user-language1
 *
 * S0 is intended for use in sign-list texts.
 *
 * NEW::this is stream_mode (or maybe stream_reln (relation) would be better?)
 * 
 * Streams also have substream tags, which consist of a hyphen plus
 * digit, e.g., S1-1.  The substream annotates or complements the main
 * stream in some way, often as a gloss:
 *
 *  SN-0 unmarked stream
 *  SN-1 variant gloss (by definition a bifurcation of the same stream);
 *       lex main column; lex equivalences implemented using other S-langs.
 *  SN-2 lex sign value (S0-2); or user-defined (SN-2 where N >= 1)
 *  SN-3 lex sign pronunciation (S0-3) or user-defined
 *  SN-4 lex sign "G" (S0-4) or user defined
 *  SN-5 lex sign name (S0-5) or user defined
 *  SN-5 lex word/phrase (i.e., equivalent in same language as SN-1)
 *  SN-6 lex contained sign
 *  SN-7 reserved
 *  SN-8 stream-oriented gloss, e.g., he2-pi
 *  SN-9 document-oriented gloss, e.g., 10-marks
 *
 * Note that ATF also has a discourse chunk mechanism which is a layer
 * between the document and streams. A year-formula, for example, is a
 * discourse feature with its own S0, not a separate stream.
 *
 * Streams can be stored in integers as small as a char; for a
 * variable S, the main stream is given by S/10; the substream is
 * given by S%10.
 */

enum gdlstream_e { GDL_S0 , GDL_S1 , GDL_S2 , GDL_S3 , GDL_S4 , GDL_S5 ,
		   GDL_S6 , GDL_S7 , GDL_S8 , GDL_S9 }; 

#endif/*GDLSTREAM_H_*/
