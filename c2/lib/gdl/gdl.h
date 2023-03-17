#ifndef GDL_H_
#define GDL_H_

enum bracket {
  e_L_squ, e_R_squ,
  e_L_cur, e_R_cur,
  e_L_par, e_R_par,
  e_L_ang, e_R_ang,
  e_L_dbl_ang, e_R_dbl_ang,
  e_L_dbl_cur, e_R_dbl_cur,
  e_L_ang_par, e_R_ang_par,
  e_L_uhs , e_R_uhs,
  e_L_lhs , e_R_lhs,
  e_L_inl_dol , e_R_inl_dol,
  e_L_inl_cmt , e_R_inl_cmt
};

struct brackets {
  int index;
  enum bracket type;
  struct brackets *next; /* only used with medial brackets */
};

struct gdl_g {
  unsigned const char *c10e; /* canonicalized grapheme */
  unsigned const char *orig; /* original grapheme */
  unsigned const char *accn; /* canonicalized grapheme mapped through accnum */
  const char *type;
  struct brackets *o;
  struct brackets *m; /* medial brackets, for legacy mode */
  struct brackets *c;
  const char *flags;
};

extern void gdl_init(void);
extern void gdl_term(void);

extern void gdl_setup_buffer(char *buf);
extern void gdlparse_string(char *s);
extern void gdlparse_init(void);
extern void gdlparse_reset(void);
extern void gdlparse_term(void);


#endif /*GDL_H_*/
