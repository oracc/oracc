static int
gvl_try_h(gvl_g *gp, gvl_g *vp, gvl_g *qp, unsigned char *q_fixed, unsigned char **mess)
{
  int qv_bad = 1;
  unsigned char *b = gvl_val_base(vp->text);
  unsigned const char *h = gvl_lookup(gvl_tmp_key(b,"h"));
  if (h)
    {
      unsigned const char *p = NULL;
      if ((p = (uccp)strstr((ccp)h, qp->oid)))
	{
	  unsigned char *p2 = NULL, *p_end = (ucp)strchr((char*)p,' '), *p_slash = NULL, *free1 = NULL, *free2 = NULL;
	  if (p_end)
	    {
	      p2 = free2 = malloc((p_end-p) + 1);
	      strncpy((char*)p2,(char*)p,p_end-p);
	      p2[p_end-p] = '\0';
	    }
	  else
	    {
	      p2 = malloc(strlen((char*)p) + 1);
	      strcpy((char*)p2,(char*)p);
	    }
	  if ((p_slash = (ucp)strchr((ccp)p,'/')))
	    {
	      p = free1 = gvl_v_from_h((uccp)b, (uccp)p_slash+1);
	      if (!p)
		{
		  fprintf(stderr, "gvl: internal error in data: gvl_from_h failed on %s\n", p_slash);
		  p = (ucp)"(null)";
		}
	    }
	  else
	    p = b;

	  /* ? build a p(qp->sign) here and set gp->text to it ? */

	  /* If the gp->text value is uppercase, make the result value
	     uppercase; then if value == qp->sign, elide the value and
	     just print the qp-sign with no parens */
	  if (gvl_v_isupper(gp->text) && !strcmp((ccp)(p=g_uc(p)), (ccp)qp->sign))
	    *mess = gvl_vmess("%s: should be %s%s", gp->text, qp->sign, QFIX);
	  else if (strcmp((ccp)vp->text,(ccp)p) || (ccp)q_fixed)
	    *mess = gvl_vmess("%s: should be %s(%s)%s", gp->text, p, qp->sign, QFIX);

	  qv_bad = 0;
	  /*ret = 1;*/ /* ok because deterministically resolved */
	  if (free1)
	    free(free1);
	  if (free2)
	    free(free2);
	}
    }
  return qv_bad;
}
