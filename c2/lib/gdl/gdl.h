#ifndef GDL_H_
#define GDL_H_

enum bracket {
  L_squ, R_squ,
  L_cur, R_cur,
  L_par, R_par,
  L_ang, R_ang,
  L_dbl_ang, R_dbl_ang,
  L_dbl_cur, R_dbl_cur,
  L_ang_par, R_ang_par,
  L_uhs , R_uhs,
  L_lhs , R_lhs,
  L_inl_dol , R_inl_dol,
  L_inl_cmt , R_inl_cmt
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

#endif /*GDL_H_*/
