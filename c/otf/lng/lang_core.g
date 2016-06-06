%{
#include <string.h>
#include "lang.h"
%}
struct langcore;
%%
sux, "020", "akk", m_sign, m_lang, c_sux, LF_BASE|LF_SAC
akk, "020", "sux", m_logo, m_lang, c_akk, LF_NORM|LF_SAC
arc, "124", "akk", m_sign, m_lang, c_arc, LF_NORM
egy, "050", "akk", m_sign, m_lang, c_egy, LF_NORM
grc, "200", "akk", m_sign, m_lang, c_grc, LF_NORM
hit, "020", "sux", m_logo, m_logo, c_hit, LF_NORM|LF_SAC
qca, "020", "qca", m_sign, m_lang, c_qca, LF_NORM|LF_SAC
qpc, "900", "qpc", m_sign, m_lang, c_qpc, LF_NORM
qpe, "938", "qpe", m_sign, m_lang, c_qpe, LF_NORM
elx, "020", "sux", m_sign, m_logo, c_elx, LF_NORM|LF_SAC
peo, "030", "peo", m_sign, m_lang, c_peo, LF_NORM
qam, "020", "qam", m_sign, m_lang, c_qam, LF_NORM|LF_SAC
qcu, "020", "qcu", m_sign, m_lang, c_qcu, LF_NORM|LF_SAC
qeb, "020", "qeb", m_sign, m_lang, c_qeb, LF_NORM|LF_SAC
xhu, "020", "xhu", m_sign, m_lang, c_xhu, LF_NORM|LF_SAC
xur, "020", "xur", m_sign, m_lang, c_xur, LF_NORM|LF_SAC
uga, "020", "uga", m_logo, m_lang, c_uga, LF_NORM|LF_SAC
qpn, "999", "qpn", m_sign, m_lang, c_qpn, LF_NORM|LF_SAC
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
