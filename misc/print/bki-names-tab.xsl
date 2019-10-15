<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:x="http://oracc.org/ns/xmd/1.0"
		>

<xsl:output method="text" encoding="UTF-8"/>

<xsl:template match="x:cat">
  <xsl:choose>
    <xsl:when test="string-length(x:id_text)>0">
      <xsl:value-of select="x:id_text"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="x:id_composite"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="x:designation"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
