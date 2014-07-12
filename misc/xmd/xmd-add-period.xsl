<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns="http://oracc.org/ns/xmd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:param name="period" select="'Neo-Assyrian'"/>

<xsl:template match="xmd:xmd">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xmd:record">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="*"/>
    <period><xsl:value-of select="$period"/></period>
  </xsl:copy>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
