#ifndef GDLSTREAM_H_
#define GDLSTREAM_H_

/* Every word in GDL is part of a stream with some streams being
 * predefined and others able to be defined by users.
 *
 * The basic stream is S0, and is the stream for the language defined
 * in #atf: lang LANGUAGE. If there is not #atf: lang this is Sumerian.
 *
 * A language shift does not necessarily change the stream: if the
 * language is a logogram language (e.g., %s or %a in Hittite context)
 * the stream does not change because the logograms are part of the
 * same language context.
 *
 * In #atf: use bilingual the second language is S1; use bilingual
 * should support giving a sequence of languages which will each be a
 * stream in order to handle multilingual texts.
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
 *		gloss, S3
 *	      if /{{[0-9]+:/ switch to the numbered stream; S0 is
 *		invalid in this context.
 *
 * {( ... )}: switch to S4 = document-oriented gloss. These include
 * 10-marks and hepi notes.
 *
 * Predefined streams:
 *
 *  S0 base
 *  S1 bilingual
 *  S2 variant gloss
 *  S3 document-oriented comment
 *
 * Note that ATF also has a discourse chunk mechanism which is a layer
 * between the document and streams. A year-formula, for example, is a
 * discourse feature with its own S0, not a separate stream.
 *
 */

#endif/*GDLSTREAM_H_*/
