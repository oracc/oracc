<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:gdf="http://oracc.org/ns/gdf/1.0"
	       version="1.0">

<xsl:output method="xml" encoding="utf-8"/>

<xsl:template match="gdf:dataset">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="*">
      <xsl:sort select="@xml:id"/>
      <xsl:copy-of select="."/>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

</xsl:transform>