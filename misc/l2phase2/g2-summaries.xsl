<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:epad="http://psd.museum.upenn.edu/epad/"
  exclude-result-prefixes="epad">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="cbd:entries">
  <summaries>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </summaries> 
</xsl:template>

<xsl:template match="cbd:letter">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="cbd:entry">
  <summary>
    <xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
    <xsl:copy-of select="@*[not(self::xml:id)]|cbd:overview/@periods"/>
    <xsl:apply-templates 
	select="cbd:cf|cbd:gw|cbd:pos|cbd:root|cbd:dt|./*/cbd:base|./*/*/cbd:mng|./*/*/cbd:term"/>
  </summary>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
