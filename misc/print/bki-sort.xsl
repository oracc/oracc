<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:x="http://oracc.org/ns/xis/1.0"
		>
  
<xsl:template match="c:senses">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="*">
      <xsl:sort select="@stemkey"/>
      <xsl:apply-templates select="."/>
    </xsl:for-each>
  </xsl:copy>    
</xsl:template>

<xsl:template match="c:forms">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="*">
      <xsl:sort select="@c" data-type="number"/>
      <xsl:apply-templates select="."/>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="c:form">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="c:t"/>
    <xsl:for-each select="x:rr">
      <xsl:sort select="@c" data-type="number"/>
      <xsl:apply-templates select="."/>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="x:rr">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="x:r">
      <xsl:sort select="@c" data-type="number"/>
      <xsl:copy-of select="."/>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
