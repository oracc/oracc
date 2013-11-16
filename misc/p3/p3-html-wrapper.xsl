<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:note="http://oracc.org/ns/note/1.0"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  exclude-result-prefixes="note xmd xst xtr xh">

<xsl:output method="xml" encoding="utf-8" indent="yes" omit-xml-declaration="yes"/>

<xsl:include href="html-standard.xsl"/>

<xsl:param name="frag-id"/>
<xsl:param name="host"/>
<xsl:param name="line-id"/>
<xsl:param name="project" select="''"/>
<xsl:param name="trans" select="'en'"/>
<xsl:param name="transonly" select="'false'"/>

<xsl:variable name="q">'</xsl:variable>

<xsl:template match="/">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
