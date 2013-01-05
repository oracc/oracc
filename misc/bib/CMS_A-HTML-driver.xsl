<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:h="http://www.w3.org/1999/xhtml"
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="t h">

<xsl:include href="biblist.xsl"/>
<xsl:include href="sort-bib.xsl"/>
<xsl:include href="CMS_A-HTML.xsl"/>

<xsl:param name="title"/>
<xsl:strip-space elements="*"/>
<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:variable name="ti">
    <xsl:choose>
      <xsl:when test="string-length($title) > 0">
        <xsl:value-of select="$title"/>
      </xsl:when>
      <xsl:when test="string-length(/t:listBibl/t:title|/*/t:listBibl/t:title) > 0">
        <xsl:value-of select="/t:listBibl/t:title|/*/t:listBibl/t:title"/>
      </xsl:when>
      <xsl:when test="string-length(/t:title|/*/t:title) > 0">
        <xsl:value-of select="/t:title|/*/t:title"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>Bibliography</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <html>
    <head>
      <link rel="stylesheet" type="text/css" 
           href="/bib-reflist.css"/>
      <title><xsl:value-of select="$ti"/></title>
    </head>
    <body>
      <xsl:apply-templates select="t:listBibl|*/t:listBibl"/>
    </body>
  </html>
</xsl:template>

<xsl:template match="t:listBibl">
  <xsl:call-template name="format-biblist">
    <xsl:with-param name="nl" select="*"/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
