<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xh="http://www.w3.org/1999/xhtml">

<xsl:strip-space elements="*"/>
<xsl:output method="text" encoding="UTF-8"/>

<xsl:template match="/">
  <xsl:text>&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xh:h3">
  <xsl:if test="not(text()='View')">
    <xsl:text>@p @b{</xsl:text>
    <xsl:value-of select="."/>
    <xsl:text>:} </xsl:text>
    <xsl:apply-templates mode="print" select="following-sibling::xh:ul[1]"/>
  </xsl:if>
</xsl:template>

<xsl:template mode="print" match="xh:ul">
  <xsl:for-each select="xh:li">
    <xsl:value-of select="."/>
    <xsl:if test="position() != last()"><xsl:text>; </xsl:text></xsl:if>
  </xsl:for-each>
  <xsl:text>&#xa;&#xa;</xsl:text>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
