#ifndef NS_ASL_H_
#define NS_ASL_H_
enum asl_a_type
{
  asl_n__,
  asl_n_sl,
  asl_n_xml,
  asl_a_base,
  asl_a_bau,
  asl_a_comment,
  asl_a_deprecated,
  asl_a_elles,
  asl_a_hex,
  asl_a_hzl,
  asl_a_id,
  asl_a_kw,
  asl_a_kwu,
  asl_a_lak,
  asl_a_lang,
  asl_a_mzl,
  asl_a_n,
  asl_a_name,
  asl_a_o,
  asl_a_obzl,
  asl_a_ogsl,
  asl_a_p,
  asl_a_project,
  asl_a_qm,
  asl_a_qn,
  asl_a_rec,
  asl_a_ref,
  asl_a_rsp,
  asl_a_sllha,
  asl_a_type,
  asl_a_uncertain,
  asl_a_utf8,
  asl_a_var,
  asl_a_varid,
  asl_a_zatu,
  asl_a_enum_top
};

enum asl_e_type
{
  asl_e_sl_form,
  asl_e_sl_inherited,
  asl_e_sl_inote,
  asl_e_sl_iv,
  asl_e_sl_list,
  asl_e_sl_note,
  asl_e_sl_pname,
  asl_e_sl_proof,
  asl_e_sl_q,
  asl_e_sl_qs,
  asl_e_sl_sign,
  asl_e_sl_signlist,
  asl_e_sl_sort,
  asl_e_sl_uname,
  asl_e_sl_unote,
  asl_e_sl_uphase,
  asl_e_sl_utf8,
  asl_e_sl_v,
  asl_e_enum_top
};

extern struct xnn_attr asl_abases[];
extern struct xnn_nstab asl_nstab[];
extern struct xnn_xname asl_anames[];
extern struct xnn_xname asl_enames[];
extern struct xnn_data asl_data;

#endif /*NS_ASLH_*/
