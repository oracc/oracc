<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:x="http://oracc.org/ns/xis/1.0"
		>

<xsl:output method="text"/>
<xsl:key name="r" match="x:r" use="@id_text"/>

<xsl:template match="/">
  <xsl:for-each select="//x:r[generate-id(.)=generate-id(key('r',@id_text))]">
    <xsl:sort/>
    <xsl:value-of select="@id_text"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:for-each>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
