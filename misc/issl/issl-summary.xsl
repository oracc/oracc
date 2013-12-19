<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:i="http://oracc.org/ns/issl"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="i">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="issl">
  <html>
    <xsl:apply-templates/>
  </html>
</xsl:template>

<xsl:template match="entry">
  <p class="summary" id="{@xml:id}">
    <span class="issl-term"><xsl:apply-templates select="term"/></span>
    <xsl:text> </xsl:text>
    <xsl:for-each select="bib/ref">
      <xsl:apply-templates select="."/>
      <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
    </xsl:for-each>
    <xsl:text>.</xsl:text>
  </p>
</xsl:template>

<xsl:template match="ref">
   <xsl:apply-templates/>
</xsl:template>

<xsl:template match="a">
  <a>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </a>
</xsl:template>

<xsl:template match="span[@class='cf']">
  <span>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </span>
</xsl:template>

<xsl:template match="span[@class='gw']">
  <span>
    <xsl:copy-of select="@*"/>
    <xsl:value-of select="."/>
  </span>
</xsl:template>

</xsl:stylesheet>
