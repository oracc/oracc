<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xis="http://oracc.org/ns/xis/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:epad="http://psd.museum.upenn.edu/epad/"
  exclude-result-prefixes="epad">

<xsl:output method="text" indent="no" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:apply-templates select=".//cbd:summary"/>
</xsl:template>

<xsl:template match="cbd:summary">
  <xsl:value-of select="@n"/><xsl:text>&#x9;</xsl:text><xsl:value-of select="@icount"/><xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="text()"/>
</xsl:stylesheet>
