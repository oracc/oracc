<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xmd="http://oracc.org/ns/xmd/1.0"
		>
<xsl:output method="text" encoding="utf-8"/>
<xsl:template match="xmd:record">
  <xsl:if test="xmd:genre='Royal Inscription'">
    <xsl:value-of select="concat(xmd:id_composite|xmd:id_text,'&#9;',xmd:external_id,'&#9;',xmd:period,'&#9;',xmd:place,'&#9;',xmd:designation,'&#xa;')"/>
  </xsl:if>
</xsl:template>
</xsl:stylesheet>
