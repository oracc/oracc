<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0">
<xsl:output method="text"/>
<xsl:template match="c:entry">
  <xsl:value-of select="concat(c:cf/@literal,'&#xa;')"/>
</xsl:template>
</xsl:stylesheet>