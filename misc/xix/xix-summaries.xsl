<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://emegir.info/index"
  xmlns:xix="http://emegir.info/index"
  xmlns:cbd="http://emegir.info/cbd"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xix">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:key name="id" match="cbd:summary" use="@xml:id"/>

<xsl:template match="xix:where">
  <xsl:variable name="ref" select="@ref"/>
  <xsl:for-each select="document('summaries.cbd',/)/*">
    <xsl:apply-templates select="key('id',$ref)"/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xix:index|xix:ix|xix:what">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy> 
</xsl:template>

<xsl:template match="cbd:summary">
  <cbd:summary>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </cbd:summary>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
