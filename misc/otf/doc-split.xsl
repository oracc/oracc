<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet version="1.0" 
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:gdl="http://oracc.org/ns/gdl/1.0"
  xmlns:norm="http://oracc.org/ns/norm/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:exsl="http://exslt.org/common"
  xmlns:md="http://oracc.org/ns/xmd/1.0"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"
  exclude-result-prefixes="xcl xff xtf gdl md xtr norm"
  extension-element-prefixes="exsl">

<xsl:param name="package"/>

<xsl:template match="office:document">
  <xsl:call-template name="subdoc">
    <xsl:with-param name="fname" select="'content'"/>
    <xsl:with-param name="tag" select="'office:document-content'"/>
    <xsl:with-param name="nodes" select="office:document-content/*"/>
  </xsl:call-template>
  <xsl:call-template name="subdoc">
    <xsl:with-param name="fname" select="'styles'"/>
    <xsl:with-param name="tag" select="'office:document-styles'"/>
    <xsl:with-param name="nodes" select="office:document-styles/*"/>
  </xsl:call-template>
  <xsl:call-template name="subdoc">
    <xsl:with-param name="fname" select="'meta'"/>
    <xsl:with-param name="tag" select="'office:document-meta'"/>
    <xsl:with-param name="nodes" select="office:meta"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="subdoc">
  <xsl:param name="fname"/>
  <xsl:param name="tag"/>
  <xsl:param name="nodes"/>
  <exsl:document href="{$package}/{$fname}.xml">
    <xsl:element name="{$tag}">
      <xsl:copy-of select="/*/@office:version"/>
      <xsl:copy-of select="$nodes"/>
    </xsl:element>
  </exsl:document>
</xsl:template>

</xsl:stylesheet>
