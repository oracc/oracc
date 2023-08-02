#ifndef NS_ASL_H_
#define NS_ASL_H_
enum asl_a_type
{
  asl_n__,
  asl_n_g,
  asl_n_n,
  asl_n_sl,
  asl_n_xml,
  asl_a_base,
  asl_a_comment,
  asl_a_deprecated,
  asl_a_form,
  asl_a_g_accented,
  asl_a_g_delim,
  asl_a_g_logolang,
  asl_a_g_pos,
  asl_a_g_role,
  asl_a_g_status,
  asl_a_g_type,
  asl_a_g_utf8,
  asl_a_hex,
  asl_a_n,
  asl_a_name,
  asl_a_o,
  asl_a_p,
  asl_a_project,
  asl_a_qm,
  asl_a_qn,
  asl_a_ref,
  asl_a_sexified,
  asl_a_sort,
  asl_a_title,
  asl_a_type,
  asl_a_uncertain,
  asl_a_xml_id,
  asl_a_xml_lang,
  asl_a_enum_top
};

enum asl_e_type
{
  asl_e_g_a,
  asl_e_g_b,
  asl_e_g_c,
  asl_e_g_d,
  asl_e_g_g,
  asl_e_g_gg,
  asl_e_g_m,
  asl_e_g_n,
  asl_e_g_nonw,
  asl_e_g_o,
  asl_e_g_p,
  asl_e_g_q,
  asl_e_g_r,
  asl_e_g_s,
  asl_e_g_v,
  asl_e_g_w,
  asl_e_n_s,
  asl_e_n_w,
  asl_e_sl_form,
  asl_e_sl_inherited,
  asl_e_sl_inote,
  asl_e_sl_iv,
  asl_e_sl_letter,
  asl_e_sl_list,
  asl_e_sl_name,
  asl_e_sl_note,
  asl_e_sl_pname,
  asl_e_sl_proof,
  asl_e_sl_q,
  asl_e_sl_qs,
  asl_e_sl_sign,
  asl_e_sl_signlist,
  asl_e_sl_signs,
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
