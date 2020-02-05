<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:xcl="http://oracc.org/ns/xcl/1.0">

<xsl:output method="text"/>
<xsl:template match="xcl:l">
  <xsl:value-of select="concat(@xml:id,'&#xa;')"/>
</xsl:template>
<xsl:template match="text()"/>
</xsl:transform>
