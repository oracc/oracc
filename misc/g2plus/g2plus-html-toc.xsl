<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:ex="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="ex"
  exclude-result-prefixes="xh xl">

<xsl:import href="html-standard.xsl"/>
<xsl:output method="xml" encoding="utf-8" indent="no"/>

<xsl:template match="xh:body">
  <toc xmlns="http://emegir.info/index" xmlns:xl="http://www.w3.org/1999/xlink">
    <xsl:attribute name="title">
      <xsl:value-of select="'ePSD Browsable HTML'"/>
    </xsl:attribute>
    <xsl:attribute name="basename">
      <xsl:value-of select="'epsd'"/>
    </xsl:attribute>
    <xsl:apply-templates select="xh:div[@class='letter']"/>
  </toc>
</xsl:template>

<xsl:template match="xh:div[@class='letter']">
  <xsl:variable name="c">
    <xsl:choose>
      <xsl:when test="@id='Ĝ'">
	<xsl:value-of select="'NG'"/>
      </xsl:when>
      <xsl:when test="@id='Š'">
	<xsl:value-of select="'SH'"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="@id"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="href" select="concat('epsd-',$c, '.html')"/>
  <xsl:choose>
    <xsl:when test="@id = 'Ĝ'">
      <letter xl:href="{$href}"><xsl:value-of select="'Ŋ'"/></letter>
    </xsl:when>
    <xsl:otherwise>
      <letter xl:href="{$href}"><xsl:value-of select="@id"/></letter>
    </xsl:otherwise>
  </xsl:choose>

  <ex:document href="{concat('website/epsd/',$href)}"
	method="xml" encoding="utf-8"
   	indent="no">
<!--
    	doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   	doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
 -->
    <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="webtype" select="'cbd'"/>
    <xsl:with-param name="with-hr" select="false()"/>
    <xsl:with-param name="with-trailer" select="false()"/>
    <xsl:with-param name="title" select="'TOC'"/>
    <xsl:with-param name="with-epsd1" select="true()"/>
    </xsl:call-template>
  </ex:document>
</xsl:template>

<xsl:template name="call-back">
  <div class="tocbanner">
    <p class="toctitle"><xsl:text>ePSD Browsable HTML</xsl:text></p>
    <p class="tocletter"><xsl:value-of select="@id"/></p>
  </div>
  <xsl:copy-of select="*/*"/>
</xsl:template>

</xsl:stylesheet>
