<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:gdl="http://oracc.org/ns/gdl/1.0"
    xmlns:nrm="http://oracc.org/ns/nrm/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns:xmd="http://oracc.org/ns/xmd/1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    xmlns:xtr="http://oracc.org/ns/xtr/1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:param name="oracc"/>
<xsl:param name="project"/>
<xsl:param name="lang"/>

<xsl:template match="/">
  <xsl:value-of select="concat('@uri=/',$project,'/cbd/',$lang,'/overview.html','&#xa;')"/>
  <xsl:value-of select="concat('@title=',$project,' ',$lang,' Glossary')"/>
  <xsl:text>&#xa;&#xa;</xsl:text>
  <xsl:value-of select="concat('&#x9;', $lang, ' gls&#xa;')"/>
</xsl:template>

</xsl:transform>