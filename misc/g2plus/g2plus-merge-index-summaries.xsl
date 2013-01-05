<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/xix/1.0"
  xmlns:xix="http://oracc.org/ns/xix/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xix">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:key name="id" match="cbd:summary" use="@ref"/>

<xsl:template match="xix:where">
  <xsl:variable name="ref" select="@ref"/>
  <xsl:for-each select="document('summaries.xml',/)/*">
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
  <summary xmlns="http://oracc.org/ns/cbd/1.0">
    <xsl:copy-of select="@*[not(name()='xml:id')]"/>
    <xsl:apply-templates/>
  </summary>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='xml:id')]"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
