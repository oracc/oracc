/*
 This file is part of the Digital Assyriologist.  Copyright
 (c) Steve Tinney, 1994,1995,1996.  It is distributed under 
 the terms of the Gnu General Public License version 2 (see
 /da/doc/COPYING or ftp://prep.ai.mit.edu:/pub/gnu/COPYING).

 $Id: concord.c,v 0.1 1996/08/16 01:20:07 s Exp $
*/

/* Revision history
 *	15 aug 1996     Very minor mods to keep GCC 2.7.2 quiet;
 *			compatibility fixes for DA library (Steve Tinney)
 * 	29 july 1994 	Minor mods to compile with GCC/Linux (Steve Tinney)
 *	16 july 1993	Added graphemic comparison function and -g option
 *			  (Steve Tinney); enlarged MAXWORD; fixed getword
 *			  and word to use unsigned char
 *      25 sept 1986    Rearranged TREENODE in preparation for adding -o
 *                        option.
 *                      Added -o option to output word index sorted by word
 *                        frequency.
 *                      (Walter Mebane)
 *      23 sept 1986    Added a compiler option (via #define WILDCARDS) to
 *                        allow all but 'outfile' file names to contain DOS
 *                        wildcard patterns.  Used CI C86 filedir() function
 *                        out of laziness.
 *                      (Walter Mebane)
 *      23 sept 1986    Minor optimizations:
 *                        reduced function calls in splay operations;
 *                        changed LINENUM from struct to union.
 *                      Now allows multiple -[i|x]FILE specifications.
 *                      Also allows multiple files to be specified as
 *                        -iFILE1+FILE2+FILE3 for indefinitely many files.
 *                      Also allows multiple input files, as in1+in2+in3,
 *                        and so on indefinitely.
 *                      (Walter Mebane)
 *      14 july 1986    Changed %D to %ld for portability.
 *                      Fixed third argument to ltoa().
 *                      (Walter Mebane)
 *      12 july 1986    Version using splay trees finally made to work.
 *                      (Walter Mebane)
 *      11 july 1986    Fixed bug with -l switch and numbers not in column 0.
 *                      Changed typedef names to upper case, after being bitten
 *                        by lnumptr.
 *                      Added completely generalized line "numbers."
 *                      (Walter Mebane)
 *      10 july 1986    Added facility for naming a file (on the command line)
 *                        that contains words that are either the only words to
 *                        be included in the index or words to be excluded
 *                        from the index.
 *                      Revised option switches and help screen accordingly.
 *                      (Walter Mebane)
 *      8 july 1986     After much thrashing, finally figured out that
 *                         FILE *fopen() was needed to run under the CI C86
 *                         big memory model.
 *                      Now letting the CI version use allocate() instead of
 *                         calloc().
 *                      Improved the help screen and error messages.
 *                      Simplified option switches (strcmpi now unnecessary).
 *                      (Walter Mebane)
 *      8 July 1986     added routines strupr, strlwr, strcmpi 
 *                        for compilers not having these 
 *                      routine "alloc" renamed "allocate"
 *                      a correction in "allocate"
 *                      now works for MTS *CBELL
 *                      (Bernard Tiffany)
 *      8 july 1986     adapted for CI86 v.2.20M;
 *                      added switch and code for anything-between-blanks
 *                      definition of "word" and for reading line numbers
 *                      of the form "10" or "10." included on each line;
 *                      added little help screen
 *                      (Walter Mebane)
 *      6 july 1986     original version (Bernard Tiffany)
*/
 
/* Language versions */

#undef DA
#define Oracc

#ifndef linux
#define linux
#endif

#ifdef __EMX__
#define GCC 1
#define UNIX 0
#include <sys/emx.h>
#endif

#ifdef linux
#define GCC 1
#define UNIX 1
#endif

#define LatticeC 0
#define LattVer2 0
#define LattVer3 0
#define MetawareC 0
#define MicrosoftC 0
#define CI_C86 0
/* define _C86_BIG for the CI C86 big memory model */
#define _C86_BIG
#define BellC 0

/* define WILDCARDS for DOS wildcard processing in filenames */
#ifdef __EMX__
#undef WILDCARDS
#define EMX_WILDCARDS
#else
#ifndef UNIX
#define WILDCARDS
#endif
#endif

#ifdef DA
#include <da_ctype.h>
#else
#ifndef Oracc
typedef unsigned char Uchar;
#endif
#endif

#ifdef Oracc
#define Uchar unsigned char
#define Boolean int
#include "collate.h"
#endif

#define LINT_ARGS 
#define DEBUG 1
#include <stdio.h>
#if (1 - BellC & 1 - CI_C86)
#include <stdlib.h>
#endif
#include <ctype128.h>
#if (1 - BellC & 1 - CI_C86)
#include <string.h>
#include <stdlib.h>
#ifndef linux
#include <process.h>
#endif
#endif
 
/* following for increasing memory available in CI C86 big model */
#if CI_C86
/* contents of ydefault.c */
/*      SET RUN TIME DEFAULT VALUES
*/
#ifdef _C86_BIG
/* _MAXFMEM=0x1800 means use up to 96K */
/* _MAXFMEM=0x4000 means use up to 256K */
/* _MAXFMEM=0x8800 means use up to 544K */
int _MAXFMEM=0x8800;            /* MAXIMUM STACK+HEAP REQUIRED, in 16 byte paragraphs */
unsigned _BUFSIZE=2048; /* BUFFER SIZE IN BIG MODEL      */
#else
int _MAXFMEM=0x1000;            /* MAXIMUM STACK+HEAP REQUIRED */
unsigned _BUFSIZE=1024; /* BUFFER SIZE IN SMALL MODEL */
#endif
int _MINFMEM=0x200;             /* MINIMUM STACK+HEAP REQUIRED */
int _MINRMEM=01;                /* RESERVED AT TOP OF MEM */
int _STAKMEM=512;               /* MIN STACK IN SBRK BEFORE REFUSING ALLOC */
/* end contents of ydefault.c */
#endif
 
#define MAXWORD 128 /*47*/
#define MAXFNAMELEN 12
#define LETTER  'a'
#define CDIGIT   '0'
#define SPACE   32
#define ALLOCSIZE 125
#define LINESIZE 79
#define NUMSIZE 12      /* reasonable, since numbers are longs */
#define SNUMSIZE 31
#define IEFOFF 0
#define IEFINCBUILD 1
#define IEFINCSKIP (-1)
#define IEFEXCBUILD 2
#define IEFEXCSKIP (-2)
#define INPUTF 5
#define DECR (-1)
#define INCR 1
#define FOREVER for(;;)
 
typedef struct blk              /* allocation buffer */
{       struct blk *nextblock;
        double *allocptr;       /* pointer to first unused block */
        int alloclen;           /* remaining length */
        double allocbuf[ALLOCSIZE];
} BLOCK, *BLOCKPTR;
 
typedef struct fnode {
        char *fname;            /* file path/name */
        struct fnode *fnext;    /* pointer to next name */
} FNODE, *FNODEPTR;
 
typedef struct flist {  /* lists of files for in1+in2 and -[i|x]FILE options */
        FNODEPTR inplink;
        FNODEPTR inclink;
        FNODEPTR exclink;
} FLIST;
 
typedef union linenum { /* line number union */
        long numnum;    /* numerical line number */
        Uchar *strnum;  /* arbitrary line "number" */
} LINENUM;
 
typedef struct lnode    /* the line number node */
{       LINENUM lnum;                   /* line number */
        struct lnode *lnodeptr; /* points to next line number */
} LNUMNODE, *LNUMPTR;
 
typedef struct wtelem { /* the basic word element */
        Uchar *word;             /* points to the text */
        unsigned int count;     /* number of occurrences */
        struct lnode *lines;    /* points to list of line nrs */
} WTELEM, *WTELEMPTR;
 
typedef struct wlist { /* linked list of word elements, for frequency sort */
        struct tnode *wtt;      /* word element structure */
        struct wlist *wlnext;   /* next wlist element */
} WLIST, *WLISTPTR;
 
typedef struct ntelem { /* the basic number element */
        struct wlist *wfirst;   /* first wlist element in linked list */
        struct wlist *wlast;    /* last wlist element in linked list */
} NTELEM, *NTELEMPTR;
 
typedef struct tnode {  /* the basic node */
        struct tnode *parent;   /* parent */
        struct tnode *left;     /* left child */
        struct tnode *right;    /* right child */
        union { /* structures of types this tree manages */
                struct wtelem wtnode;   /* word element structure */
                struct ntelem ntnode;   /* number element structure */
        } telems;
} TREENODE, *TREEPTR, **TREEPTRPTR;
 
#define WTNODE telems.wtnode
#define NTNODE telems.ntnode
 
#if CI_C86
#define void int
#endif
 
#if (MicrosoftC | MetawareC | LattVer3 | defined(linux)) 
int main (int, char *[]);
char *allocate (unsigned int);
TREEPTR buildindex (TREEPTR, TREEPTRPTR, LINENUM *, int *, int);
void closefiles (void);
TREEPTR drotate (TREEPTR,TREEPTR,TREEPTR);
void error (char *, int);
void fileerror (char *, char *);
void flistadd (char *, int, FLIST *);
void getargs (int, char *[]);
LINENUM *getlinenum (Uchar *, LINENUM *, int);
int getword (Uchar *, int);
void helpscreen (int);
void concord_index (void);
LNUMPTR lnuminit (LINENUM *);
void memerror (int);
TREEPTR numtsearch (TREEPTR, unsigned int);
void numtadd (TREEPTR);
void numtree (TREEPTR);
TREEPTR splay (TREEPTR);
TREEPTR srotate (TREEPTR, TREEPTR);
TREEPTR treebuild (TREEPTR, Uchar *, LINENUM *);
void treeout (WTELEMPTR);
void treeprint (TREEPTR);
TREEPTR treesearch (TREEPTR, Uchar *);
int type (int);
#else
int main();
char *allocate();
TREEPTR buildindex();
void closefiles();
TREEPTR drotate();
void error();
void fileerror();
void flistadd();
void getargs();
LINENUM *getlinenum();
int getword();
void helpscreen();
void concord_index();
LNUMPTR lnuminit();
void memerror();
TREEPTR numtsearch();
void numtadd();
void numtree();
TREEPTR splay();
TREEPTR srotate();
TREEPTR treebuild();
void treeout();
void treeprint();
TREEPTR treesearch();
int type();
#endif

#ifdef linux
#undef strcmp
#endif
 
int
lstrcmp (const void *k1, const void *k2)
{
  return strcmp (k1, k2);
}

#define strcmp ustrcmp
int ustrcmp (const void *k1, const void *k2);
int lstrcmp (const void *k1, const void *k2); /* can't use graphcmp for line numbers */

#ifdef WILDCARDS
extern char *filedir();
#endif
 
#if BellC||GCC
#define ltoa itoa
extern char *strlwr(), *strupr();
#endif
 
#if CI_C86
extern long atoi();
extern char *malloc();
extern FILE *fopen();
extern char *strchr(), *strrchr(), *strlwr(), *strupr();
#endif
 
FILE *Infile, *Outfile, *Reffile;
BLOCKPTR Firstblock, Lastblock;
FLIST Files;
int Uniqwords, Wordcount;
int Dexcfile, Dincfile, Docase, Doindex, Dofout, Dofreq, Dowords, Linemode;
#ifdef Oracc
int do_TeX, do_graphemic_sort, do_line_breaks, do_silent_running;
#endif
Uchar Linebuf[LINESIZE], Errmsg[100];
TREEPTR Numptree;

int main(argc, argv)   /* word frequency count and assorted indexes */
int argc;
char *argv[];
{
        BLOCKPTR tempptr, blkptr;

        Infile = stdin;
        Outfile = stdout;
        Reffile = NULL;
        Files.inplink = Files.inclink = Files.exclink = NULL;
        Firstblock = Lastblock = NULL;
        Dowords = Docase = Doindex = Dofreq = 1;
        Dofout = Linemode = 0;
        Dexcfile = Dincfile = IEFOFF;
        Numptree = NULL;
        getargs (argc, argv);
        Uniqwords = Wordcount = 0;
#ifdef Oracc
#if 1
	collate_init((unsigned const char*)"unicode");
#else
	graphcmp_init (Docase ? GCMP_FOLDCASE : GCMP_NOFOLDCASE, GCMP_OBEY_DELIMS);
#endif
#endif
        concord_index();
        blkptr = Firstblock;
        while (blkptr != NULL)
        {       tempptr = blkptr;
                blkptr = blkptr->nextblock;
                free ((char *) tempptr);
        }
        closefiles();
	return 0;
}
 
char *allocate(s)
unsigned int s;
{       char *p;
        unsigned int len;
        BLOCKPTR newblock;
 
        len = (s+sizeof(double)-1) / sizeof(double);
        if (len > ALLOCSIZE)
                error("Internal error: Block too large to allocate.",0);
        if (Lastblock == NULL || len > Lastblock->alloclen)
        {       newblock = (BLOCKPTR) malloc (sizeof(BLOCK));
                if (newblock == NULL)
                        memerror (0);
                if (Firstblock == NULL)
                        Firstblock = newblock;
                else
                        Lastblock->nextblock = newblock;
                Lastblock = newblock;
                Lastblock->allocptr = Lastblock->allocbuf;
                Lastblock->alloclen = ALLOCSIZE;
                Lastblock->nextblock = NULL;
        }
        p = (char *) Lastblock->allocptr;
        Lastblock->allocptr += len;
        Lastblock->alloclen -= len;
        return (p);
}
 
TREEPTR buildindex(p0,p1p,lp,wcntptr,exclflag)
TREEPTR p0;
TREEPTRPTR p1p;
LINENUM *lp;
int *wcntptr, exclflag;
{
        Uchar word[MAXWORD];
        int t, type = 0;
 
        if (exclflag == IEFEXCSKIP && Linemode == 0)    type = 1;
        else if (exclflag == IEFEXCSKIP)                type = 2;
        else if (Linemode == 0)                         type = 3;
        else                                            type = 4;
 
        FOREVER {
                if (type == 2 || type == 4) {
                        if ((lp = getlinenum(word, lp,
                                Linemode == 2 ? SNUMSIZE : NUMSIZE)) == NULL)
                                break;
                }
                while ((t = getword(word, MAXWORD)) != EOF)
                        if (t == LETTER) {
                                (*wcntptr)++;
#ifdef Oracc
				if (!do_graphemic_sort)
#endif
				  {
                                    if (Docase == 1)        strlwr(word);
                                    else if (Docase == 2)   strupr(word);
				  }
                                if (type > 2
                                        || strcmp((*p1p = splay(treesearch(*p1p, word)))->WTNODE.word, word))
                                        p0 = treebuild(p0, word, lp);
                        }
                        else if (t == '\n')
			  {
			    if (type == 1 || type == 3)
			      lp->numnum++;
			    else
			      break;
			  }
                if (type == 1 || type == 3)
                        lp->numnum--;
                if (t == EOF)   break;
        }
 
        return p0;
}
 
void closefiles()
{
        if (Infile != NULL && Infile != stdin) fclose (Infile);
        if (Outfile != NULL && Outfile != stdout) fclose (Outfile);
        if (Reffile != NULL && Reffile != stdout && Reffile != stdin)
                fclose (Reffile);
}
 
TREEPTR drotate(p,pv,pw)        /* a double splay rotation */
TREEPTR p, pv, pw;
{
        int type = 0;
 
        if ((p == pv->left && pv == pw->right)
                || (p == pv->right && pv == pw->left))    type = 1;
        else if ((p == pv->left && pv == pw->left)
                || (p == pv->right && pv == pw->right))   type = 2;
        else {
                fprintf(stderr,"Bad pointers in tree:\n");
                fprintf(stderr,"p %ld, p->p %ld, p->l %ld, p->r %ld\n",
                        (long)p,(long)p->parent,(long)p->left,(long)p->right);
                fprintf(stderr,"pv %ld, pv->p %ld, pv->l %ld, pv->r %ld\n",
                        (long)pv,(long)pv->parent,(long)pv->left,(long)pv->right);
                fprintf(stderr,"pw %ld, pw->p %ld, pw->l %ld, pw->r %ld\n",
                        (long)pw,(long)pw->parent,(long)pw->left,(long)pw->right);
                error("(drotate)",0);
        }
        if (type == 1) {
                if (p == pv->left) {
                        pw->right = p->left;
                        if (p->left != NULL) p->left->parent = pw;
                        pv->left = p->right;
                        if (p->right != NULL) p->right->parent = pv;
                        p->left = pw;
                        p->right = pv;
                }
                else { /* (p == pv->right) */
                        pw->left = p->right;
                        if (p->right != NULL) p->right->parent = pw;
                        pv->right = p->left;
                        if (p->left != NULL) p->left->parent = pv;
                        p->right = pw;
                        p->left = pv;
                }
        }
        else {
                if (p == pv->left) {
                        pv->left = p->right;
                        if (p->right != NULL) p->right->parent = pv;
                        p->right = pv;
                        pw->left = pv->right;
                        if (pv->right != NULL) pv->right->parent = pw;
                        pv->right = pw;
                }
                else { /* (p == pv->right) */
                        pv->right = p->left;
                        if (p->left != NULL) p->left->parent = pv;
                        p->left = pv;
                        pw->right = pv->left;
                        if (pv->left != NULL) pv->left->parent = pw;
                        pv->left = pw;
                }
        }
        p->parent = pw->parent;
        if (pw->parent != NULL)
	  {
	    if (pw == pw->parent->left) pw->parent->left = p;
	    else if (pw == pw->parent->right) pw->parent->right = p;
	    else {
	      fprintf(stderr,"Bad pointers in tree:\n");
	      fprintf(stderr,"pw %ld, pw->par->l %ld, pw->par->r %ld",
		      (long)pw,(long)pw->parent->left,(long)pw->parent->right);
	      error("(drotate 1)",0);
	    }
	  }
	pv->parent = p;
        pw->parent = type == 1 ? p : pv;
        return p;
}
 
void error (msg,niceflag)               /* print an error and quit */
char *msg;
int niceflag;
{
        if (niceflag) closefiles();
        fprintf (stderr,
                niceflag ? "(concord)  %s\n\nEnter 'concord -?' for help.\n"
                         :  "(concord)  %s\n",
                msg);
        exit (1);
}
 
void fileerror (ftypep, fnamep)
char *ftypep, *fnamep;
{
  sprintf ((char*)Errmsg,"Invalid %s file:  %s",ftypep,fnamep);
  error ((char*)Errmsg,1);
}
 
void flistadd(fnamep, dtype, flistp)    /* build lists of files */
char *fnamep;
int dtype;
FLIST *flistp;
{
        char *s;
        FNODEPTR fnop, fnop2;
#ifdef WILDCARDS
        int plen;
        char *wfl, *wf;
        FNODEPTR wfnop, wfnext;
#endif
 
        while (*fnamep) {
                if (NULL != (s = strchr(fnamep,'+'))) *s = '\0';
#ifdef WILDCARDS
                if (strchr(fnamep,'*') || strchr(fnamep,'?')) {
                        if ((wfl = filedir (fnamep,0x00)) == NULL) {
                                sprintf(Errmsg,"No files matched %s",fnamep);
                                error(Errmsg, 1);
                        }
                        else {
                                if ((wf = strrchr(fnamep,'\\')))
                                        plen = strlen(fnamep)-strlen(wf+1);
                                else
                                        plen = 0;
                                for (wf = wfl, fnop = NULL; *wf != NULL;) {
                                        wfnop = (FNODEPTR) allocate (sizeof(FNODEPTR));
                                        wfnop->fnext = NULL;
                                        wfnop->fname = allocate(plen+MAXFNAMELEN+1);
                                        if (plen) strncpy(wfnop->fname, fnamep, plen);
                                        strcpy(wfnop->fname+plen, wf);
                                        if (fnop == NULL)
                                                fnop = wfnop;
                                        else {
                                                for (wfnext = fnop; wfnext->fnext != NULL; wfnext = wfnext->fnext) ;
                                                wfnext->fnext = wfnop;
                                        }
                                        wf = wf + strlen(wf) + 1;
                                }
                                free (wfl);
                        }
                }
                else {
                        fnop = (FNODEPTR) allocate (sizeof(FNODEPTR));
                        fnop->fnext = NULL;
                        strcpy((fnop->fname = allocate(strlen(fnamep)+1)), fnamep);
                }
#else
                fnop = (FNODEPTR) allocate (sizeof(FNODEPTR));
                fnop->fnext = NULL;
                strcpy((fnop->fname = allocate(strlen(fnamep)+1)), fnamep);
#endif
                if (dtype == IEFINCBUILD)
		  {
		    if (flistp->inclink == NULL) flistp->inclink = fnop;
		    else {
		      for (fnop2 = flistp->inclink; fnop2->fnext != NULL; fnop2 = fnop2->fnext) ;
		      fnop2->fnext = fnop;
		    }
		  }
                else if (dtype == IEFEXCBUILD)
		  {
		    if (flistp->exclink == NULL) flistp->exclink = fnop;
		    else {
		      for (fnop2 = flistp->exclink; fnop2->fnext != NULL; fnop2 = fnop2->fnext) ;
		      fnop2->fnext = fnop;
		    }
		  }
                else if (dtype == INPUTF)
		  {
		    if (flistp->inplink == NULL) flistp->inplink = fnop;
		    else {
		      for (fnop2 = flistp->inplink; fnop2->fnext != NULL; fnop2 = fnop2->fnext) ;
		      fnop2->fnext = fnop;
		    }
		  }
                if (s == NULL) break;
                while (*++s == '+') ;
                fnamep = s;
        }
}
 
void getargs(argc, argv)        /* get arguments */
int argc;
char *argv[];
{       char *s, t;
        int df = IEFOFF, filecount = 0;
        FNODEPTR fnop;
 
        while (--argc)
        {       
#if UNIX
	  if ((t = (*++argv)[0]) == '-') {
#else 
	    /* support MS-DOS style options */
	  if ((t = (*++argv)[0]) == '-' || t == '/') {
#endif
                        for (s = argv[0]; df == IEFOFF && *++s;)
                                switch (*s) {
                                case 'F' :
                                case 'f' : Dofreq = 0; break;
                                case 'N' :
                                case 'n' : Doindex = 0; break;
                                case 'C' :
                                case 'c' : Docase = 0; break;
                                case 'O' :
                                case 'o' :
                                        Dofout = s[1] == '+' ? (++s, INCR) : DECR;
                                        break;
                                case 'U' :
                                case 'u' : Docase = 2; break;
                                case 'W' :
                                case 'w' : Dowords = 0; break;
                                case 'L' :
                                case 'l' :
                                        Linemode = s[1] == '*' ? (++s, 2) : 1;
                                        break;
                                case '?' : helpscreen(0); break;
                                case 'I' :
                                case 'i' :
                                        flistadd(++s, df = IEFINCBUILD, &Files);
                                        break;
                                case 'X' :
                                case 'x' :
                                        flistadd(++s, df = IEFEXCBUILD, &Files);
                                        break;
#if 1 /*SJT Oracc*/
                                case 'B' :
                                case 'b' : do_line_breaks = 0; break;
                                case 'G' :
                                case 'g' : do_graphemic_sort = 1; break;
                                case 'S' :
                                case 's' : do_silent_running = 1; break;
                                case 'T' :
                                case 't' : do_TeX = 1; break;
#endif
                                default :
                                        fprintf (stderr,
                                                "Illegal option-- %s\n\n", s);
                                        helpscreen(1);
                                        break;
                                }
                        df = IEFOFF;
                }
                else {
                        filecount++;
                        if (filecount == 1)
                                flistadd(argv[0], INPUTF, &Files);
                        else if (filecount == 2) {
                                Outfile = fopen (argv[0], "w");
                                if (Outfile == NULL)
                                        fileerror("output",argv[0]);
                        }
                        else
                                error ("Too many files specified.",1);
                }
        }
 
        /* check input files before doing anything time consuming */
        for (fnop = Files.inplink; fnop != NULL; fnop = fnop->fnext)
                if ((Infile = fopen (fnop->fname, "r")) == NULL)
                        fileerror("input",fnop->fname);
                else if (Infile != stdout && Infile != stdin)
                        fclose (Infile);
        for (fnop = Files.inclink; fnop != NULL; fnop = fnop->fnext)
                if ((Reffile = fopen (fnop->fname, "r")) == NULL)
                        fileerror("include",fnop->fname);
                else if (Reffile != stdout && Reffile != stdin)
                        fclose (Reffile);
        for (fnop = Files.exclink; fnop != NULL; fnop = fnop->fnext)
                if ((Reffile = fopen (fnop->fname, "r")) == NULL)
                        fileerror("exclude",fnop->fname);
                else if (Reffile != stdout && Reffile != stdin)
                        fclose (Reffile);
        Reffile = NULL;
}
 
LINENUM *getlinenum(w, lp, lim)   /* look for a line number of form "10. " or "10 " */
Uchar *w;
LINENUM *lp ;
int lim;
{
        int c, t;
        Uchar *w0 = NULL;
 
        if (Linemode == 2)
                w = lp->strnum;
        else
                w0 = w;
 
        while ((t = type(c = fgetc(Infile))) == SPACE) ;
 
        if (c == EOF)
                return NULL;
        else if (Linemode == 2)         /* anything not blank accepted */
                *w++ = c;
        else if (t != CDIGIT) {          /* not a CDIGIT, push back */
                ungetc(c,Infile);
                return lp;
        }
        else                            /* must be a CDIGIT, accept */
                *w++ = c;
 
        while (--lim > 0) {
                t = type(c = *w++ = fgetc(Infile));
                if (Linemode == 2 ? t == SPACE : t != CDIGIT && c != '.') {
                        ungetc(c,Infile) ;
                        break ;
                }
                else if (Linemode != 2 && c == '.') --w ;
        }
        if ((lim = 0)) {  /* skip past too long part of line 'numbers' */
                if (Linemode == 2)
                        while (type(c = fgetc(Infile)) != SPACE) ;
                else
                        while (type(c = fgetc(Infile)) == CDIGIT || c == '.') ;
                ungetc(c,Infile);
        }
 
        *(w-1) = '\0';
        if (Linemode != 2)
	  lp->numnum = atoi((char*)w0) ;
 
        return lp ;
}
int 
getword(w, lim)         /* get next word from input */
Uchar *w;
int lim;
{       int c, t;
 
        t = type(*w++ = c = fgetc(Infile));
        if (Dowords ? t != LETTER : t == SPACE || c == EOF)
        {       *w = '\0';
                return (c);
        }
        while (--lim > 0) 
        {       t = type(*w++ = c = fgetc(Infile));
                if (Dowords ? t != LETTER && t != CDIGIT : t == SPACE || c == EOF)
                {       ungetc(c,Infile);
                        break;
                }
        }
        *(w-1) = '\0';
        return (LETTER);
}
 
void helpscreen(val)
int val;
{
        printf( "     CONCORDANCE PROGRAM (1986) by Walter Mebane and Bernard Tiffany\n");
        printf( " concord [-fnucwol?] infile[+inf2] outfile [-iFILE[+FILE]] [-xFILE[+FILE]]\n");
        printf( "                  outfile omitted:  write output to standard output.\n");
        printf( "    infile(s) and outfile omitted:  read input from standard input.\n");
#ifdef WILDCARDS
        printf( "          All filenames except for outfile can contain wildcards.\n");
#endif
        printf( " -f       Do not include word frequency counts in output.\n");
        printf( " -n       Do not write line number indexes for each word.\n");
        printf( " -u       Character conversion to upper case (default is to lower case).\n");
        printf( " -c       No case conversion ('abc' is not the same as 'ABC').\n");
        printf( " -w       Define 'word' as any whitespace-bounded string of characters.\n");
        printf( " -o (-o+) Sort output by decreasing (increasing) word frequency.\n");
        printf( " -l       Use line numbers included in the input file at the beginning of\n");
        printf( "          lines of text;  numbers must be entirely digits (%d digits max).\n", NUMSIZE-1);
        printf( "          a line not beginning with a number will get the most recent number.\n");
        printf( "          (default is to set line numbers by counting newline characters).\n");
        printf( " -l*      Like -l, except that the first non-blank text (up to %d characters)\n", SNUMSIZE);
        printf( "          in each line will be used as the line 'number' for that line.\n");
        printf( " -?       Display this help screen.\n");
        printf( " -iFILE   Only words included in FILE(s) will be indexed.\n");
        printf( " -xFILE   Words appearing in FILE(s) will be excluded from the index.");

#ifdef Oracc
        printf( "\n -b       Don't generate line breaks in lists of occurrences.\n");
        printf( " -g       Grapheme-based sort.\n");
        printf( " -s       Silent running.\n");
        printf( " -t       Do TeX output.\n");
#endif
 
        closefiles();
        exit(val);
}
 
void concord_index()    /* do the indexing */
{
        int tdoindex, tlinemode;
        FILE *tmpfile;
        FNODEPTR fnop;
        LINENUM *linenp;
        TREEPTR root0 = NULL, root1 = NULL;
 
        linenp = (LINENUM *) allocate(sizeof(LINENUM));
        if (Linemode == 1)              linenp->numnum = 1L;
        else if (Linemode == 2)         linenp->strnum = (unsigned char*)allocate(SNUMSIZE);
 
        /* exclusion files */
        for (fnop = Files.exclink; fnop != NULL; fnop = fnop->fnext) {
                if ((Reffile = fopen (fnop->fname, "r")) == NULL)
                        fileerror("exclude",fnop->fname);
                Dexcfile = IEFEXCBUILD;
                tdoindex = Doindex;
                tlinemode = Linemode;
                tmpfile = Infile;
                Doindex = 0;
                Linemode = 1;
                Infile = Reffile;
                root1 = buildindex(root1, NULL, linenp, &Wordcount, IEFOFF);
                Doindex = tdoindex;
                Linemode = tlinemode;
                Infile = tmpfile;
                Uniqwords = Wordcount = 0;
                if (Reffile != stdout && Reffile != stdin) fclose (Reffile);
                Reffile = NULL;
                Dexcfile = IEFEXCSKIP;
        }
        /* inclusion files */
        for (fnop = Files.inclink; fnop != NULL; fnop = fnop->fnext) {
                if ((Reffile = fopen (fnop->fname, "r")) == NULL)
                        fileerror("include",fnop->fname);
                Dincfile = IEFINCBUILD;
                tlinemode = Linemode;
                tmpfile = Infile;
                Linemode = 1;
                Infile = Reffile;
                root0 = Dexcfile == IEFEXCSKIP ?
                        buildindex(root0, &root1, linenp, &Wordcount, IEFEXCSKIP) :
                        buildindex(root0, NULL, linenp, &Wordcount, IEFOFF);
                Linemode = tlinemode;
                Infile = tmpfile;
                Uniqwords = Wordcount = 0;
                if (Reffile != stdout && Reffile != stdin) fclose (Reffile);
                Reffile = NULL;
                Dincfile = IEFINCSKIP;
        }
 
        if (Linemode != 2) linenp->numnum = 1L;
 
        for (fnop = Files.inplink; fnop != NULL || Infile == stdin; fnop = fnop->fnext) {
                if (fnop != NULL)
                        if ((Infile = fopen (fnop->fname, "r")) == NULL)
                                fileerror("input",fnop->fname);
                root0 = Dexcfile == IEFEXCSKIP ?
                        buildindex(root0, &root1, linenp, &Wordcount, IEFEXCSKIP) :
                        buildindex(root0, NULL, linenp, &Wordcount, IEFOFF);
                if (Infile != stdout && Infile != stdin) fclose (Infile);
                Infile = NULL;
                if (fnop == NULL) break;
                if (fnop->fnext != NULL && Linemode != 2) linenp->numnum++;
        }
 
	if (!do_silent_running)
	  fprintf(stderr,
		  Linemode == 0
		  ? "%d words in input, %d distinct words indexed, %ld lines in input\n"
		  : "%d words in input, %d distinct words indexed\n",
		  Wordcount, Uniqwords, linenp->numnum);
        treeprint(Dofout ? (numtree(root0), Numptree) : root0);
}
 
LNUMPTR lnuminit(lp)
LINENUM *lp;
{
        LNUMPTR p1;
 
        p1 = (LNUMPTR) allocate(sizeof(LNUMNODE));
        if (Linemode == 2) {
	  p1->lnum.strnum = (unsigned char*)allocate(strlen((char*)lp->strnum)+1);
	  strcpy((char*)p1->lnum.strnum,(char*)lp->strnum);
        }
        else
                p1->lnum.numnum = lp->numnum;
        p1->lnodeptr = NULL;
 
        return p1;
}
 
#define LNUMCOMP(s,p) \
        (Linemode == 2  ? lstrcmp(s.strnum,p->strnum) : s.numnum != p->numnum)
 
void memerror (niceflag)
int niceflag;
{
        error("Out of memory.",niceflag);
}
 
void numtadd (wp)       /* add a new entry to the number tree */
TREEPTR wp;
{
        unsigned int n = 0;
        TREEPTR p, p0;
 
        p = numtsearch(Numptree, wp->WTNODE.count);
 
                        /* p is at or above match point */
        if (p == NULL ||
                (n = p->NTNODE.wfirst->wtt->WTNODE.count) != wp->WTNODE.count) {
                p0 = (TREEPTR) allocate(sizeof(TREENODE));
                p0->NTNODE.wlast = p0->NTNODE.wfirst =
                        (WLISTPTR) allocate (sizeof(WLIST));
                p0->left = p0->right = NULL;
                p0->parent = p;
 
                if (p == NULL)          p = p0;
                else if ((Dofout == DECR && n < wp->WTNODE.count)
                        || (Dofout == INCR && n > wp->WTNODE.count))
                        p->left = p0;
                else    p->right = p0;
 
                p = p0;
        }
        else {  /* at match point */
                p->NTNODE.wlast = p->NTNODE.wlast->wlnext =
                        (WLISTPTR) allocate (sizeof(WLIST));
        }
        p->NTNODE.wlast->wtt = wp;
        p->NTNODE.wlast->wlnext = NULL;
 
        Numptree = splay(p);
}
 
void numtree (wp)               /* make a tree ordered by word counts */
TREEPTR wp;
{
        /*TREEPTR np = NULL;*/
 
        if (wp != NULL) {
                numtree (wp->left);
                if (wp->WTNODE.count != 0) numtadd (wp);
                numtree (wp->right);
        }
}
 
TREEPTR numtsearch (p,n)        /* return node p at or just above number n */
TREEPTR p;
unsigned int n;
{
        unsigned int n0;
        TREEPTR p0 = NULL;
 
        while (p != NULL && n != (n0 = p->NTNODE.wfirst->wtt->WTNODE.count)) {
                p0 = p;
                if ((Dofout == DECR && n0 < n) || (Dofout == INCR && n0 > n))
                        p = p->left;
                else
                        p = p->right;
        }
        return p == NULL ? p0 : p;
}
 
TREEPTR splay(p)        /* make p the root via splay steps */
TREEPTR p;
{
        TREEPTR pv, pw;
 
        if (p != NULL)
	  {
	    while ((pv = p->parent) != NULL)
	      {
		if ((pw = pv->parent) == NULL)  srotate(p,pv);
		else                            drotate(p,pv,pw);
	      }
	  }
        return p;
}
 
TREEPTR srotate(p,pv)   /* a single splay rotation */
TREEPTR p, pv;
{
        if (p == pv->left) {
                pv->left = p->right;
                if (p->right != NULL) p->right->parent = pv;
                p->right = pv;
        }
        else if (p == pv->right) {
                pv->right = p->left;
                if (p->left != NULL) p->left->parent = pv;
                p->left = pv;
        }
        else {
                fprintf(stderr,"Bad pointers in tree:\n");
                fprintf(stderr,"p %ld, pv %ld, pv->l %ld, pv->r %ld",
                        (long)p,(long)pv,(long)pv->left,(long)pv->right);
                error("(srotate).",0);
        }
        pv->parent = p;
        p->parent = NULL;       /* note -- assuming pv->parent was NULL */
        return p;
}
 
TREEPTR treebuild(p,w,lp)               /* install w at or below p */
TREEPTR p;
Uchar *w;
LINENUM *lp;
{       int cond = 0;
        TREEPTR p0;
        LNUMPTR temp;
 
        p = treesearch(p,w);    /* p is at or above match point */
 
        if (p == NULL || (cond = strcmp(w,p->WTNODE.word)) != 0) {
                if (Dincfile == IEFINCSKIP) /* skip word not in inclusion tree */
                        goto IEFJUMPPOINT; /* note that IEFINCSKIP is set only after inc. tree has been built */
                Uniqwords++;
 
                p0 = (TREEPTR) allocate(sizeof(TREENODE));
                p0->WTNODE.word = (unsigned char *)allocate(strlen((char*)w)+1);
                strcpy((char*)p0->WTNODE.word, (char*)w);
                p0->WTNODE.count = Dincfile == IEFINCBUILD ? 0 : 1;
                p0->WTNODE.lines = NULL;
                p0->left = p0->right = NULL;
                p0->parent = p;
                if (Doindex && Dincfile != IEFINCBUILD)
                        p0->WTNODE.lines = lnuminit(lp);
 
                if (p == NULL)          p = p0;
                else if (cond < 0)      p->left = p0;
                else                    p->right = p0;
 
                p = p0;
        }
        else {  /* at match point */
                if (Dincfile == IEFINCBUILD)
                        goto IEFJUMPPOINT; /* skip repeats when building inclusion tree */
                if (p->WTNODE.count == 0)
                        Uniqwords++;    /* needed when using IEFINC tree */
                p->WTNODE.count++;
                if (Doindex) {
                        if (p->WTNODE.lines == NULL && Dincfile == IEFINCSKIP)
                                p->WTNODE.lines = lnuminit(lp);
                        else {
                                temp = p->WTNODE.lines; 
                                while (temp->lnodeptr != NULL
                                        && LNUMCOMP(temp->lnum,lp))
                                        temp = temp->lnodeptr;
                                if (LNUMCOMP(temp->lnum,lp))
                                        temp->lnodeptr = lnuminit(lp);
                        }
                }
        }
IEFJUMPPOINT:
        splay(p);
/*      while (p->parent != NULL) p = p->parent;*/ /* alternative to splay */
        return p;
}
 
void treeout (wp)               /* output a tree element */
WTELEMPTR wp;
{
        Uchar numbuf[NUMSIZE], *nb;
        int linelen, numlen;
        LNUMPTR temp;
 
        if (Dofreq)
	  sprintf((char*)Linebuf, "%s (%u)", wp->word, wp->count);
        else
	  strcpy((char*)Linebuf, (char*)wp->word);
#ifdef Oracc
	if (do_TeX)
	  {
  	    fprintf(Outfile, "\\Cnc{");
  	    strcat((char*)Linebuf, "}{");
	  }
#endif
        linelen = strlen((char*)Linebuf);
        for (temp = wp->lines; temp != NULL; temp = temp->lnodeptr) {
                if (Linemode == 2)
                        nb = temp->lnum.strnum;
                else {
#if GCC
		  sprintf ((char*)numbuf, "%ld", temp->lnum.numnum);
#else
                        ltoa (temp->lnum.numnum, numbuf, 10);
#endif
                        nb = numbuf;
                }
                numlen = strlen ((char*)nb);
                if (numlen+linelen+1 > LINESIZE) {
			if (do_line_breaks)
			  {
                            fprintf (Outfile, "%s\n", Linebuf);
                            strcpy ((char*)Linebuf, "    ");
                            strcat((char*)Linebuf, (char*)nb);
                            linelen = numlen+4;
			  }
			else
			  {
                            fprintf (Outfile, "%s ", Linebuf);
                            strcpy ((char*)Linebuf, (char*)nb);
                            linelen = numlen;
			  }
                }
                else {
                        strcat((char*)Linebuf, " ");
                        strcat((char*)Linebuf, (char*)nb);
                        linelen += numlen+1;
                }
        }
#ifdef Oracc
	if (do_TeX)
          fprintf(Outfile, "%s}\n", Linebuf);
	else
#endif
          fprintf(Outfile, "%s\n", Linebuf);
}
 
void treeprint (p)              /* print tree p recursively */
TREEPTR p;
{
        if (p != NULL) {
                treeprint(p->left);
 
                if (Dofout) {
                        WLISTPTR wlp;
                        for (wlp = p->NTNODE.wfirst; wlp != NULL;
                                wlp = wlp->wlnext)
                                treeout (&wlp->wtt->WTNODE);
                }
                else if (p->WTNODE.count != 0)
                        treeout (&p->WTNODE);
 
                treeprint(p->right);
        }
}
 
TREEPTR treesearch(p,w)  /* return node p at or just above w */
TREEPTR p;
Uchar *w;
{
        int cond;
        TREEPTR p0 = NULL;
 
        while (p != NULL && (cond = strcmp(w, p->WTNODE.word)) != 0) {
                p0 = p;
                if (cond < 0)   p = p->left;
                else            p = p->right;
        }
        return p == NULL ? p0 : p;
}
 
int type(c)                         /* return type of character */
int c;
{
/* Stupid *CBELL */
#if BellC
        if (c == EOF)
           return (EOF);
#endif
        if (isalpha(c) != 0 || c == '_')
                return (LETTER);
        else if (isdigit(c) != 0)
                return (CDIGIT);
        else if (isspace(c) != 0)
                return (SPACE);
        else
                return (c);
}
 
#if (BellC | CI_C86 | defined(linux))
 
char *strlwr(s)
char *s;
{       register char *p;
        p = s;
        while (*s != '\0')
        {       *s = tolower(*s);
                s++;
        }
        return (p);
}
 
char *strupr(s)
char *s;
{       register char *p;
        p = s;
        while (*s != '\0')
        {       *s = toupper(*s);
                s++;
        }
        return (p);
}
#endif

int
ustrcmp (const void *k1, const void *k2)
{
  const unsigned char *s1 = k1, *s2 = k2;

#ifdef Oracc
  if (do_graphemic_sort)
#if 1
    return collate_cmp_graphemes(s1,s2);
#else    
    return graphcmp (s1, s2);
#endif
#endif

  while (*s1 == *s2 && *s1)
    {
      ++s1;
      ++s2;
    }
  if (*s1 > *s2)
    return 1;
  else if (*s1 < *s2)
    return -1;
  else
    return 0;
}

#ifdef Oracc
const char *prog = "concord";
int major_version = 1, minor_version = 0;
const char *usage_string = "[-abciknt] <files>";
void
help ()
{
  helpscreen (0);
}
int
opts (int o, char *op)
{
  return 0;
}

#endif
