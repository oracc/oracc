<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:xpd="http://oracc.org/ns/xpd/1.0">
<xsl:output method="text"/>
<xsl:template match="xpd:option">
  <xsl:value-of select="concat(@name,'&#x9;',@value,'&#xa;')"/>
</xsl:template>
<xsl:template match="text()"/>
</xsl:transform>
