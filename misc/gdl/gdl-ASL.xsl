<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:norm="http://oracc.org/ns/norm/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xl="http://www.w3.org/1999/xlink"
  exclude-result-prefixes="g norm xcl xff xtf xl">

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="g:c|g:n|g:q">
  <xsl:value-of select="concat(@form,'&#xa;')"/>
</xsl:template>

<xsl:template match="g:p|g:s|g:v">
  <xsl:value-of select="concat(text(),'&#xa;')"/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
