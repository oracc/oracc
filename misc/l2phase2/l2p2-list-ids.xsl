<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:for-each select="//@xml:id">
    <xsl:if test="not(starts-with(.,'gdl'))">
      <xsl:value-of select="."/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
