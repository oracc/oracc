<?xml version='1.0'?>

<xsl:stylesheet version="1.0"
  xmlns="http://oracc.org/ns/xix/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:epad="http://psd.museum.upenn.edu/epad/"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="g2plus-index-driver.xsl"/>
<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:call-template name="make-index">
    <xsl:with-param name="basename" select="'signnames'"/>
    <xsl:with-param name="title" select="'Sumerian Sign-name Index'"/>
    <xsl:with-param name="node-list" select="//cbd:base/cbd:s"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="make-sortkey">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template name="make-what">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template name="make-where">
  <xsl:value-of select="concat(ancestor::cbd:entry/cbd:cf,'[',ancestor::cbd:entry/cbd:gw,']')"/>
</xsl:template>

<xsl:template name="make-href">
  <xsl:value-of select="concat(ancestor::cbd:entry/@xml:id,'.html')"/>
</xsl:template>

</xsl:stylesheet>
