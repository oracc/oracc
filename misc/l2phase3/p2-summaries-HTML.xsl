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
  <p class="toc-entry">
    <a href="{concat($jsResultsBegin,'/',/*/@project,$summaries-file,$jsEnd)}">
      <xsl:value-of select="@dc:title"/>
    </a>
  </p>
  <ex:document href="{concat('./',$webdir,$summaries-file)}"
	       method="xml" encoding="utf-8"
	       doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
	       doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
	       indent="yes">
    <xsl:call-template name="make-html">
      <xsl:with-param name="project" select="/*/@project"/>
      <xsl:with-param name="with-hr" select="false()"/>
      <xsl:with-param name="with-trailer" select="false()"/>
      <xsl:with-param name="title" select="Summary"/>
      <xsl:with-param name="webtype" select="'p2-p1'"/>
    </xsl:call-template>
  </ex:document>
</xsl:template>

<xsl:template name="call-back">

<!--
  <xsl:if test="not(self::cbd:letter)">
    <h1 class="toc"><xsl:value-of 
    select="/*/@project"/>/<xsl:value-of 
    select="/*/@xml:lang"/></h1>
    
    <p class="toc-entry">
      <a href="{concat($jsResultsBegin,'/',/*/@project,'/cbd/',/*/@xml:lang,'/overview.html',$jsEnd)}"
	 >Overview</a>
    </p>
  </xsl:if>
-->
  
  <div>
    <xsl:apply-templates/>
  </div>

<!--
  <xsl:if test="not(self::cbd:letter)">
    <p class="toc-entry">
      <a href="{concat($jsResultsBegin,'/',/*/@project,'/cbd/',/*/@xml:lang,'/onebigfile.html',$jsEnd)}"
	 >One Big File</a>
    </p>
  </xsl:if>
  
 -->
</xsl:template>

</xsl:stylesheet>
