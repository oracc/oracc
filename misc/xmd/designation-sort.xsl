<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xmd="http://oracc.org/ns/xmd/1.0"
		>
<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:for-each select="//xmd:record">
    <xsl:sort select="xmd:designation"/>
    <xsl:value-of select="concat(xmd:id_composite|xmd:id_text,'&#9;',position(),'&#xa;')"/>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
