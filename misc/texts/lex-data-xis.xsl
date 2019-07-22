<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:template match="lex:group[@type='word' or @type='phra' or @type='equi']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="xis">
      <xsl:for-each select=".//@xis">
	<xsl:value-of select="."/>
	<xsl:if test="not(position()=last())">
	  <xsl:text> </xsl:text>
	</xsl:if>
      </xsl:for-each>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
