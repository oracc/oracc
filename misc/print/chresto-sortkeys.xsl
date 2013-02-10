<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0">
<xsl:output method="text"/>
<xsl:template match="c:entry">
  <xsl:value-of select="concat(substring-before(@n,'['),'&#xa;')"/>
</xsl:template>
<xsl:template match="text()"/>
</xsl:stylesheet>
