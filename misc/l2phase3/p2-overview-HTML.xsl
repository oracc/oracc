<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:norm="http://oracc.org/ns/norm/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="cbd"
  xmlns:ex="http://exslt.org/common"
  extension-element-prefixes="cbd dc ex g norm xtf">

<xsl:import href="html-standard.xsl"/>
<xsl:import href="g2-gdl-HTML.xsl"/>
<xsl:import href="p2-summary-HTML-lib.xsl"/>

<xsl:param name="basename" select="/*/@project"/>
<xsl:param name="project" select="/*/@project"/>
<xsl:param name="webdir" select="'02www/cbd'"/>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:variable name="lower" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="upper" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:variable name="jsArticleBegin">javascript:p3Article('</xsl:variable>
<xsl:variable name="jsResultsBegin">javascript:p3Letter('</xsl:variable>
<xsl:variable name="jsEnd">')</xsl:variable>

<xsl:template match="/">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="with-hr" select="false()"/>
    <xsl:with-param name="with-trailer" select="false()"/>    
    <xsl:with-param name="title" select="TOC"/>
    <xsl:with-param name="webtype" select="'p2-p1'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="cbd:letter">
  <xsl:variable name="c" select="@dc:title"/>
  <xsl:variable name="summaries-file">
    <xsl:value-of select="concat('/cbd/',/*/@xml:lang,'/summaries-',$c,'.html')"/>
  </xsl:variable>
  <div class="summary-letter">
    <h1>
      <xsl:value-of select="@dc:title"/>
    </h1>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template name="call-back">
  <xsl:apply-templates/>  
</xsl:template>

</xsl:stylesheet>
