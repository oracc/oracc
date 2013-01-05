<?xml version='1.0' encoding="utf-8"?>
<xsl:stylesheet version="1.0" 
  xmlns:xtl="http://oracc.org/ns/list/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xmd xtl xtr">

<xsl:param name="project"/>
<xsl:output method="xml" omit-xml-declaration="yes" indent="yes"/>

<xsl:template match="xtl:list">
  <div class="xmdoutline bb-4 bt-4 br-1">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="xtl:group">
  <div class="xmdoutline" id="{@type}">
    <h3 class="h3"><xsl:value-of select="@type"/></h3>
    <div class="xol2">
      <xsl:apply-templates/>
    </div>
  </div>
</xsl:template>

<xsl:template match="xtl:item">
  <xsl:variable name="type">
    <xsl:choose>
      <xsl:when test="starts-with(@text,'P')">exemplar</xsl:when>
      <xsl:when test="starts-with(@text,'Q')">composite</xsl:when>
      <xsl:otherwise/>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="image-nodes" select="document(concat(@path,'/',@text,'.xmd'))/*/xmd:cat/xmd:images/*"/>
  <xsl:variable name="image" select="count($image-nodes)"/>
<!--
  <xsl:variable name="best-image">
    <xsl:choose>
      <xsl:when test="$image-nodes[@type='p']">
	<xsl:text>image</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$image-nodes[@type='l' and @scale='full']/@src"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
 -->
  <xsl:variable name="imgclass">
    <xsl:choose>
      <xsl:when test="$image > 0">withimg</xsl:when>
      <xsl:otherwise>sansimg</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <p class="xo {$imgclass}">
    <xsl:choose>
      <xsl:when test="$image > 0">
	<a target="_blank" href="http://oracc.museum.upenn.edu/{$project}/{@text}/image" class="haveimg">IMG</a>
      </xsl:when>
      <xsl:otherwise>
	<span class="noimg">&#xa0;</span>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="@proj = 'cdli'">
	<a href="javascript:showcdli('{@text}')">
	  <xsl:value-of select="@n"/>
	</a>
      </xsl:when>
      <xsl:otherwise>
	<a href="javascript:showexemplar('{@proj}','{@text}','','')">
	  <xsl:value-of select="@n"/>
	</a>
      </xsl:otherwise>
    </xsl:choose>
  </p>
</xsl:template>

</xsl:stylesheet>
