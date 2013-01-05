<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0">

<xsl:template match="*[@g:file|@g:line|@g:me]">
  <g>
    <xsl:copy-of select="@g:file|@g:line|@g:me"/>
    <xsl:choose>
      <xsl:when test="./text()">
	<xsl:value-of select="."/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </g>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>