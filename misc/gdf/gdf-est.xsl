<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
	       xmlns:gdf="http://oracc.org/ns/gdf/1.0"
	       version="1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="gdf:entry">
  <xsl:value-of select="concat('@title=', /gdf:dataset/@gdf:title, ': ', ./@gdf:title, '&#xa;')"/>
  <xsl:value-of select="concat('@uri=/', $project, '/data/', /gdf:dataset/@gdf:abbrev, '/', ./@xml:id, '&#xa;&#xa;')"/>
  <xsl:for-each select="*">
    <xsl:for-each select="*|text()">
      <xsl:apply-templates select="."/>
      <xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
    </xsl:for-each>
    <xsl:text>&#xa;</xsl:text>
  </xsl:for-each>
    <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

</xsl:transform>