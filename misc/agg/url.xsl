<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    >
<xsl:template name="url-name">
  <xsl:choose>
    <xsl:when test="string-length(xpd:url)>0">
      <xsl:value-of select="xpd:url"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="concat('./',@n)"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>
</xsl:stylesheet>
