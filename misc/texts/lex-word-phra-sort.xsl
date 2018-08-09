<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:template match="lex:group[@type='word']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates>
      <xsl:sort select="@value"/>
    </xsl:apply-templates>
  </xsl:copy>
</xsl:template>

<xsl:template match="lex:group[@type='phra']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates>
      <xsl:sort select="@value"/>
    </xsl:apply-templates>
  </xsl:copy>
</xsl:template>

<xsl:template match="lex:group[@type='equi']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates>
      <xsl:sort select="@value"/>
    </xsl:apply-templates>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
