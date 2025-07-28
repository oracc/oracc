<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:eb="http://oracc.org/ns/esp-biblatex/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:h="http://www.w3.org/1999/xhtml"
    version="1.0" 
    >

  <xsl:template match="eb:cite">
    <xsl:if test="not(@star='*')">
      <xsl:text>[</xsl:text>
      <xsl:value-of select="@key"/>
      <xsl:if test="@pp">
	<xsl:text>: </xsl:text>
	<xsl:value-of select="@pp"/>
      </xsl:if>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="eb:references"/>
  
</xsl:stylesheet>
