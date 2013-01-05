<xsl:stylesheet 
    xmlns:gdl="http://oracc.org/ns/gdl/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0">
<xsl:output method="text" encoding="utf-8"/>
<xsl:template match="/">
  <xsl:for-each select="//@gdl:utf8">
    <xsl:value-of select="."/>
    <xsl:if test="not(position()=last())">
      <xsl:text> </xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>