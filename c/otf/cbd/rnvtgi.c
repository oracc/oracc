/* rnvtgi: Driver for rnv validation of T(ext) G(lossary) I(nfosets) */

extern int (*er_printf)(char *format,...);
extern int (*er_vprintf)(char *format,...);
extern locator *xo_loc;
#define xvh_err(msg) msglist_averr(xo_loc,(msg),ap);
static void tgi_verror_handler(int erno,va_list ap)
{
  if(erno&ERBIT_RNL)
    {
      rnl_verror_handler(erno&~ERBIT_RNL,ap);
    }
  else
    {
      const char *xphase = phase;
      phase = "rnv";
      switch(erno)
	{
	case RNV_ER_ELEM: xvh_err("tag %s^%s not allowed"); break;
	case RNV_ER_AKEY: xvh_err("tag modifier %s^%s not allowed"); break;
	case RNV_ER_AVAL: xvh_err("tag modifier %s^%s with invalid value \"%s\""); break;
	case RNV_ER_EMIS: xvh_err("incomplete content"); break;
	case RNV_ER_AMIS: xvh_err("missing attributes of %s^%s"); break;
	case RNV_ER_UFIN: xvh_err("unfinished content of tag %s^%s"); break;
	case RNV_ER_TEXT: xvh_err("invalid data or text not allowed"); break;
	case RNV_ER_NOTX: xvh_err("text not allowed"); break;
	default: assert(0);
	}
      phase = xphase;
    }
}

void
rnvtgi_init(struct xnn_data *xdp)
{
  rnvval_init(tgi_verror_handler, xdp);
}
