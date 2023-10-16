#ifndef SL_H_
#define SL_H_
enum sl_a_type
{
  sl_n__,
  sl_n_sl,
  sl_n_xml,
  sl_a_base,
  sl_a_bau,
  sl_a_comment,
  sl_a_deprecated,
  sl_a_elles,
  sl_a_hex,
  sl_a_hzl,
  sl_a_id,
  sl_a_kw,
  sl_a_kwu,
  sl_a_lak,
  sl_a_lang,
  sl_a_mzl,
  sl_a_n,
  sl_a_name,
  sl_a_o,
  sl_a_obzl,
  sl_a_ogsl,
  sl_a_p,
  sl_a_project,
  sl_a_qm,
  sl_a_qn,
  sl_a_rec,
  sl_a_ref,
  sl_a_rsp,
  sl_a_sllha,
  sl_a_type,
  sl_a_uncertain,
  sl_a_utf8,
  sl_a_var,
  sl_a_varid,
  sl_a_zatu,
  sl_a_enum_top
};

enum sl_e_type
{
  sl_e_sl_form,
  sl_e_sl_inherited,
  sl_e_sl_inote,
  sl_e_sl_iv,
  sl_e_sl_list,
  sl_e_sl_note,
  sl_e_sl_pname,
  sl_e_sl_proof,
  sl_e_sl_q,
  sl_e_sl_qs,
  sl_e_sl_sign,
  sl_e_sl_signlist,
  sl_e_sl_sort,
  sl_e_sl_uname,
  sl_e_sl_unote,
  sl_e_sl_uphase,
  sl_e_sl_utf8,
  sl_e_sl_v,
  sl_e_enum_top
};

extern struct xnn_attr sl_abases[];
extern struct xnn_nstab sl_nstab[];
extern struct xnn_xname sl_anames[];
extern struct xnn_xname sl_enames[];

#endif /*SLH_*/
