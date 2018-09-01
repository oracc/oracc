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
	<xsl:if test="@rws">
	  <xsl:text> </xsl:text>
	  <span class="rws">
	    <xsl:text>(</xsl:text><xsl:value-of select="@rws"/><xsl:text>)</xsl:text>
	  </span>
	</xsl:if>
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

</xsl:stylesheet>
