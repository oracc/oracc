%{
#include <string.h>
#include "lang.h"
%}
struct langcore;
%%
sux, "020", "akk", m_sign, m_lang, c_sux, LF_BASE|LF_SAC
akk, "020", "sux", m_logo, m_lang, c_akk, LF_NORM|LF_SAC
arc, "124", "akk", m_sign, m_lang, c_arc, LF_NORM
egy, "050", "", m_sign, m_lang, c_egy, 
grc, "200", "akk", m_sign, m_lang, c_grc, LF_NORM
hit, "020", "sux", m_logo, m_logo, c_hit, LF_NORM|LF_SAC|LF_LOGO
qca, "020", "qca", m_sign, m_lang, c_qca, LF_NORM|LF_SAC
qpc, "900", "qpc", m_sign, m_lang, c_qpc, LF_NORM
qpe, "938", "qpe", m_sign, m_lang, c_qpe, LF_NORM
elx, "020", "sux", m_sign, m_logo, c_elx, LF_BASE|LF_SAC
peo, "030", "peo", m_logo, m_lang, c_peo, LF_NORM
plq, "020", "plq", m_logo, m_lang, c_plq, LF_NORM
qam, "020", "qam", m_sign, m_lang, c_qam, LF_NORM|LF_SAC
qka, "020", "qka", m_sign, m_lang, c_qka, LF_NORM|LF_SAC
qcu, "020", "qcu", m_sign, m_lang, c_qcu, LF_NORM|LF_SAC
qeb, "020", "qeb", m_sign, m_lang, c_qeb, LF_NORM|LF_SAC
xht, "020", "sux", m_logo, m_logo, c_xht, LF_NORM|LF_SAC|LF_LOGO
xhu, "020", "sux", m_logo, m_logo, c_xhu, LF_NORM|LF_SAC|LF_LOGO
xur, "020", "sux", m_logo, m_logo, c_xur, LF_NORM|LF_SAC|LF_LOGO
qur, "980", "sux", m_logo, m_logo, c_qur, 
xlu, "020", "xlu", m_logo, m_logo, c_xlu, LF_NORM|LF_SAC
hlu, "080", "hlu", m_logo, m_logo, c_hlu, 
uga, "020", "uga", m_logo, m_lang, c_uga, LF_NORM|LF_SAC
qpn, "999", "qpn", m_sign, m_lang, c_qpn, LF_NORM|LF_SAC
xco, "109", "xco", m_logo, m_logo, c_qam, LF_NORM|LF_LOGO
%%
struct langcore *
langcore_of(const char *lang)
{
  static char base[4];
  char*b = base;
  const char *tmp;
  if ((tmp = lang))
    {
      while (*tmp && '-' != *tmp)
        {
          *b++ = *tmp++;
          if (b - base == 3)
            break;
        }
      *b = '\0';
      return lang_core(base,3);
    }
  return NULL;
}
