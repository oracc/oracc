/* Cut down and slightly refactored from pcre2demo.c as accessed from github 20230816 */

#define PCRE2_CODE_UNIT_WIDTH 8

#include <stdio.h>
#include <string.h>
#include <list.h>
#include <pool.h>
#include <pcre2if.h>

static int pcre2if_trace = 0;

extern struct pcre2if_m *pcre2if_save_match(PCRE2_SPTR subject, PCRE2_SIZE *ovector, int i, Pool *poolp);

pcre2_code *
pcre2if_set_pattern(unsigned const char *pattern)
{
  pcre2_code *re;
  int errornumber;
  PCRE2_SIZE erroroffset;
  
  re = pcre2_compile(
		     (PCRE2_SPTR)pattern,   /* the pattern */
		     PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
		     0,                     /* default options */
		     &errornumber,          /* for error number */
		     &erroroffset,          /* for error offset */
		     NULL);                 /* use default compile context */
  
  if (re == NULL)
    {
      PCRE2_UCHAR buffer[256];
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));      
      fprintf(stderr, "[pcre2if] PCRE2 compilation failed at offset %d: %s\n",
	      (int)erroroffset, buffer);
      return NULL;
    }
  else if (pcre2if_trace)
    fprintf(stderr, "[pcre2if] PCRE2 compilation succeeded\n");
  
  return re;
}

List *
pcre2if_match(pcre2_code *re, const unsigned char *subject, int find_all, Pool *poolp, char sentinel)
{
  int i;
  int rc;
  int crlf_is_newline;
  uint32_t newline;
  uint32_t option_bits;
  size_t subject_length = strlen((ccp)subject);
  PCRE2_SIZE *ovector;
  pcre2_match_data *match_data;
  List *ml = NULL;

  match_data = pcre2_match_data_create_from_pattern(re, NULL);

  rc = pcre2_match(
		   re,                   /* the compiled pattern */
		   (PCRE2_SPTR)subject,  /* the subject string */
		   subject_length,    	 /* the length of the subject */
		   0,                    /* start at offset 0 in the subject */
		   0,                    /* default options */
		   match_data,           /* block for storing the result */
		   NULL);                /* use default match context */

  /* Matching failed: handle error cases */
  if (rc < 0)
    {
      switch(rc)
	{
	case PCRE2_ERROR_NOMATCH:
	  if (pcre2if_trace)
	    fprintf(stderr, "[pcre2if] No match\n");
	  break;
	default:
	  fprintf(stderr, "[pcre2if] Matching error %d\n", rc);
	  break;
	}
      pcre2_match_data_free(match_data);   /* Release memory used for the match */
      return NULL;
    }

  ml = list_create(LIST_SINGLE);
   
  /* Match succeded. Get a pointer to the output vector, where string offsets are
     stored. */
  
  ovector = pcre2_get_ovector_pointer(match_data);
  if (pcre2if_trace)
    fprintf(stderr, "\n[pcre2if] Match succeeded at offset %d\n", (int)ovector[0]);

  /*************************************************************************
   * We have found the first match within the subject string. If the output *
   * vector wasn't big enough, say so. Then output any substrings that were *
   * captured.                                                              *
   *************************************************************************/
  
  /* The output vector wasn't big enough. This should not happen, because we used
     pcre2_match_data_create_from_pattern() above. */  
  if (rc == 0)
    fprintf(stderr, "[pcre2if] ovector was not big enough for all the captured substrings\n");
  
  for (i = 0; i < rc; i++)
    list_add(ml, pcre2if_save_match(subject, ovector, i, poolp));

  if (!find_all)
    {
      pcre2_match_data_free(match_data);
      return ml;
    }

  (void)pcre2_pattern_info(re, PCRE2_INFO_ALLOPTIONS, &option_bits);
  int utf8 = (option_bits & PCRE2_UTF) != 0;
  
  /* Now find the newline convention and see whether CRLF is a valid newline
     sequence. */
  (void)pcre2_pattern_info(re, PCRE2_INFO_NEWLINE, &newline);
  crlf_is_newline = newline == PCRE2_NEWLINE_ANY ||
    newline == PCRE2_NEWLINE_CRLF ||
    newline == PCRE2_NEWLINE_ANYCRLF;

  for (;;)
    {
      uint32_t options = 0;                    /* Normally no options */
      PCRE2_SIZE start_offset = ovector[1];  /* Start at end of previous match */
      
      /* If the previous match was for an empty string, we are finished if we are
	 at the end of the subject. Otherwise, arrange to run another match at the
	 same point to see if a non-empty match can be found. */
      
      if (ovector[0] == ovector[1])
	{
	  if (ovector[0] == subject_length) break;
	  options = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
	}

      /* If there is a sentinel char check that start_offset is
	 looking at one and move back to restart the match against the
	 final sentinel of the preceding match */
      if (sentinel && sentinel != subject[start_offset])
	--start_offset;
      
      /* Run the next matching operation */
      
      rc = pcre2_match(
		       re,                   /* the compiled pattern */
		       subject,              /* the subject string */
		       subject_length,       /* the length of the subject */
		       start_offset,         /* starting offset in the subject */
		       options,              /* options */
		       match_data,           /* block for storing the result */
		       NULL);                /* use default match context */
      
      /* This time, a result of NOMATCH isn't an error. If the value in "options"
	 is zero, it just means we have found all possible matches, so the loop ends.
	 Otherwise, it means we have failed to find a non-empty-string match at a
	 point where there was a previous empty-string match. In this case, we do what
	 Perl does: advance the matching position by one character, and continue. We
	 do this by setting the "end of previous match" offset, because that is picked
	 up at the top of the loop as the point at which to start again.
	 
	 There are two complications: (a) When CRLF is a valid newline sequence, and
	 the current position is just before it, advance by an extra byte. (b)
	 Otherwise we must ensure that we skip an entire UTF character if we are in
	 UTF mode. */      
      if (rc == PCRE2_ERROR_NOMATCH)
	{
	  if (options == 0) break;                    /* All matches found */
	  ovector[1] = start_offset + 1;              /* Advance one code unit */
	  if (crlf_is_newline &&                      /* If CRLF is newline & */
	      start_offset < subject_length - 1 &&    /* we are at CRLF, */
	      subject[start_offset] == '\r' &&
	      subject[start_offset + 1] == '\n')
	    ovector[1] += 1;                          /* Advance by one more. */
	  else if (utf8)                              /* Otherwise, ensure we */
	    {                                         /* advance a whole UTF-8 */
	      while (ovector[1] < subject_length)       /* character. */
		{
		  if ((subject[ovector[1]] & 0xc0) != 0x80) break;
		  ovector[1] += 1;
		}
	    }
	  continue;    /* Go round the loop again */
	}

      /* Other matching errors are not recoverable. */      
      if (rc < 0)
	{
	  printf("Matching error %d\n", rc);
	  pcre2_match_data_free(match_data);
	  list_free(ml, NULL);
	  return NULL;
	}
      
      /* Match succeded */
      
      if (pcre2if_trace)
	fprintf(stderr, "\n[pcre2if] Match succeeded again at offset %d\n", (int)ovector[0]);
      
      /* The match succeeded, but the output vector wasn't big enough. This
	 should not happen. */
      
      if (rc == 0)
	printf("[pcre2if] ovector was not big enough for all the captured substrings\n");
      
      /* As before, show substrings stored in the output vector by number */
      for (i = 0; i < rc; i++)
	list_add(ml, pcre2if_save_match(subject, ovector, i, poolp));
    }

  if (pcre2if_trace)
    fprintf(stderr, "\n");

  pcre2_match_data_free(match_data);

  return ml;
}

struct pcre2if_m *
pcre2if_save_match(PCRE2_SPTR subject, PCRE2_SIZE *ovector, int i, Pool *poolp)
{
  PCRE2_SPTR substring_start = subject + ovector[2*i];
  size_t substring_length = ovector[2*i+1] - ovector[2*i];

  struct pcre2if_m *pmp = calloc(1, sizeof(struct pcre2if_m));
  pmp->mstr = (ucp)pool_alloc(substring_length+1, poolp);
  strncpy((char *)pmp->mstr, (ccp)substring_start, substring_length);

  pmp->mstr[substring_length] = '\0';
  pmp->off = (size_t)ovector[2*i];
  pmp->len = (size_t)substring_length;

  if (pcre2if_trace)
    fprintf(stderr, "[pcre2if] %2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
  
  return pmp;
}
