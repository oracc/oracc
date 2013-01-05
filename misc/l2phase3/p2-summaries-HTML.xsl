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

<xsl:param name="basename" select="/*/@project"/>
<xsl:param name="project" select="/*/@project"/>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:variable name="lower" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="upper" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:variable name="jsArticleBegin">javascript:cbdArticleFromContent('</xsl:variable>
<xsl:variable name="jsResultsBegin">javascript:cbdResultsFromOutline('</xsl:variable>
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
  <xsl:variable name="c">
    <xsl:choose>
      <xsl:when test="@dc:title='Ĝ'"><xsl:value-of select="'GN'"/></xsl:when>
      <xsl:when test="@dc:title='Š'"><xsl:value-of select="'SH'"/></xsl:when>
      <xsl:when test="@dc:title='Ś'"><xsl:value-of select="'SS'"/></xsl:when>
      <xsl:when test="@dc:title='Ŋ'"><xsl:value-of select="'GN'"/></xsl:when>
      <xsl:when test="@dc:title='Ṣ'"><xsl:value-of select="'ST'"/></xsl:when>
      <xsl:when test="@dc:title='Ṭ'"><xsl:value-of select="'TT'"/></xsl:when>
      <xsl:when test="@dc:title='Ş'"><xsl:value-of select="'ST'"/></xsl:when>
      <xsl:when test="@dc:title='Ţ'"><xsl:value-of select="'TT'"/></xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="@dc:title"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
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

<xsl:template match="cbd:summary">
  <p class="summary" id="{@ref}">
    <xsl:variable name="js">
      <xsl:text>'</xsl:text>
      <xsl:value-of select="@ref"/>
      <xsl:text>.html'</xsl:text>
    </xsl:variable>
    <span class="summary">
      <span class="summary-headword">
	<xsl:variable name="href">
	  <xsl:value-of select="concat($jsArticleBegin,
	    				'/',$project,'/cbd/',/*/@xml:lang,'/',@ref,'.html',
					$jsEnd)"/>
	</xsl:variable>
	<a href="{$href}">
	  <xsl:apply-templates select="cbd:cf|cbd:gw|cbd:pos|cbd:root"/>
	</a>
	<xsl:if test="@icount">
	  <xsl:text> (</xsl:text>
	  <xsl:value-of select="@icount"/>
	  <xsl:text>x)</xsl:text>
	</xsl:if>
	<xsl:text> </xsl:text>
      </span>
      <xsl:value-of select="@periods"/><xsl:text> </xsl:text>
      <xsl:apply-templates select="cbd:base|cbd:mng|cbd:term"/>
    </span>
  </p>
</xsl:template>

<xsl:template match="cbd:cf">
  <span class="cf"><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="cbd:gw">
  <xsl:text> [</xsl:text>
  <span class="gw"><xsl:value-of select="translate(text(),$lower,$upper)"/></span>
  <xsl:text>]</xsl:text>
</xsl:template>

<xsl:template match="cbd:pos">
  <xsl:text> </xsl:text>
  <span class="cf"><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="cbd:root">
  <xsl:value-of select="concat(' (&#x221A;',text(),')')"/>
</xsl:template>

<xsl:template match="cbd:base">
  <xsl:choose>
    <xsl:when test="not(preceding-sibling::cbd:base)">
      <xsl:text> wr. </xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>; </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <span class="wr"><xsl:apply-templates select="cbd:text/*"/></span>
<!--
  <xsl:if test="not(following-sibling::cbd:base)">
    <xsl:text>.</xsl:text>
  </xsl:if>
 -->
</xsl:template>

<xsl:template match="cbd:mng">
  <xsl:choose>
    <xsl:when test="not(preceding-sibling::cbd:mng)">
      <xsl:text> "</xsl:text>
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>; </xsl:text>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="not(following-sibling::cbd:mng)">
    <xsl:text>"</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="cbd:term">
  <xsl:choose>
    <xsl:when test="not(preceding-sibling::cbd:term)">
      <xsl:text> Akk.&#xa0;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>; </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <span class="akk"><xsl:apply-templates/></span>
  <xsl:if test="not(following-sibling::cbd:term)">
<!--    <xsl:text>.</xsl:text> -->
  </xsl:if>
</xsl:template>

<xsl:template match="cbd:sup">
  <sup><xsl:apply-templates/></sup>
</xsl:template>

<xsl:template name="call-back">

  <xsl:if test="not(self::cbd:letter)">
    <h1 class="toc"><xsl:value-of 
    select="/*/@project"/>/<xsl:value-of 
    select="/*/@xml:lang"/></h1>
    
    <p class="toc-entry">
      <a href="{concat($jsResultsBegin,'/',/*/@project,'/cbd/',/*/@xml:lang,'/overview.html',$jsEnd)}"
	 >Overview</a>
    </p>
  </xsl:if>
  
  <xsl:apply-templates/>

  <xsl:if test="not(self::cbd:letter)">
    <p class="toc-entry">
      <a href="{concat($jsResultsBegin,'/',/*/@project,'/cbd/',/*/@xml:lang,'/onebigfile.html',$jsEnd)}"
	 >One Big File</a>
    </p>
  </xsl:if>
  
</xsl:template>

</xsl:stylesheet>
