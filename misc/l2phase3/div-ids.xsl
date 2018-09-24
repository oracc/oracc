<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:ex="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="ex"
  exclude-result-prefixes="xh xl dc cbd">

<xsl:output method="text" encoding="UTF-8"/>
  
<xsl:template match="xh:div[@id][*][not(@class='letter')]">
  <xsl:if test="string-length(@id) > 0">
    <xsl:value-of select="ancestor-or-self::*[@class='body']/@xml:id"/>
    <xsl:text>&#9;</xsl:text>
    <xsl:value-of select="@id"/>
    <xsl:text>&#xa;</xsl:text>
    <xsl:apply-templates/>
  </xsl:if>
</xsl:template>

<xsl:template match="text()"/>
  
</xsl:stylesheet>
