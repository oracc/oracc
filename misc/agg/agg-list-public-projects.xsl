<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text" encoding="utf8"/>
<xsl:template match="/*">
  <xsl:for-each select="*/@n">
    <xsl:value-of select="concat(., '&#xa;')"/>
  </xsl:for-each>
</xsl:template>
</xsl:transform>