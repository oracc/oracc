<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:x="http://oracc.org/ns/xis/1.0"
		>

<xsl:param name="xis" select="'/Users/stinney/orc/cmawro/01bld/akk/akk.xis'"/>

<xsl:template match="c:form">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="c:t"/>
    <xsl:variable name="xis-id" select="@xis"/>
    <xsl:for-each select="document($xis)">
      <xsl:for-each select="id($xis-id)/x:r">
	<xsl:copy>
	  <xsl:copy-of select="@*"/>
	  <xsl:attribute name="id_text">
	    <xsl:value-of select="substring-before(text(),'.')"/>
	  </xsl:attribute>
	  <xsl:value-of select="."/>
	</xsl:copy>
      </xsl:for-each>
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
