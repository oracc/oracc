<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:gdf="http://oracc.org/ns/gdf/1.0"
	       version="1.0">
<xsl:output method="text" encoding="utf-8"/>
<xsl:template match="gdf:entry">
  <xsl:value-of select="concat(@xml:id, '&#xa;')"/>
</xsl:template>
</xsl:transform>