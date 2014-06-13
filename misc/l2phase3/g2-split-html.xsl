<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:ex="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xh"
  extension-element-prefixes="ex"
>

<xsl:import href="html-standard.xsl"/>

<xsl:param name="subdir"/>
<xsl:param name="webdir"/>

<xsl:variable name="subdir-str">
  <xsl:choose>
    <xsl:when test="string-length($subdir) > 0">
      <xsl:value-of select="concat($subdir,'/')"/>
    </xsl:when>
    <xsl:otherwise/>
  </xsl:choose>
</xsl:variable>

<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>

<xsl:template match="/">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xh:head"/>

<xsl:template match="xh:body">
  <xsl:apply-templates select="*/xh:div[@class='body']"/>
</xsl:template>

<xsl:template match="xh:div[@class='body']">
  <xsl:variable name="outfile" select="concat('./',$webdir,'/cbd/',$subdir-str,@xml:id,'.html')"/>
<!--  <xsl:message>gsplit div[class=body] outfile=<xsl:value-of select="$outfile"/></xsl:message> -->
  <ex:document href="{$outfile}"
    method="xml" encoding="utf-8"
    doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
    indent="yes">
    <xsl:call-template name="make-html">
      <xsl:with-param name="project" select="$project"/>
      <xsl:with-param name="webtype" select="'cbd'"/>
      <xsl:with-param name="with-hr" select="false()"/>
      <xsl:with-param name="with-trailer" select="false()"/>
      <xsl:with-param name="body-class" select="ancestor::xh:body/@class"/>
    </xsl:call-template>
  </ex:document>
</xsl:template>

<xsl:template name="call-back">
  <xsl:copy-of select="*"/>
</xsl:template>

</xsl:stylesheet>
