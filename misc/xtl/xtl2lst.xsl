<?xml version='1.0' encoding="utf-8"?>
<xsl:stylesheet version="1.0" 
  xmlns:xtl="http://oracc.org/ns/list/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xmd xtl xtr">

<xsl:param name="project"/>
<xsl:output method="text" encoding="utf8-8"/>

<xsl:template match="xtl:group[@type='Sources']">
  <xsl:for-each select="xtl:item">
    <xsl:value-of select="@text"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
