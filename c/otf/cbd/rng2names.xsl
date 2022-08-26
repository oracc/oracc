<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:r="http://relaxng.org/ns/structure/1.0"
		version="1.0">

<xsl:output method="text"/>

<xsl:template match="r:grammar">
  <xsl:if test="@ns">
    <xsl:value-of select="concat('n&#x9;',@ns,'&#xa;')"/>
  </xsl:if>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="r:attribute">
  <xsl:value-of select="concat('a&#x9;',@name,'&#xa;')"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="r:element">
  <xsl:value-of select="concat('e&#x9;',@name,'&#xa;')"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
