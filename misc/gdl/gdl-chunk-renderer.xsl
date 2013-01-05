<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="g">

<!-- 
This module cannot be used alone; it should be used in
conjunction with gdl-HTML.xsl.  By default gdl-HTML.xsl does fast
rendering of breakage and status by using the g:o and g:c attributes;
this does not work when rendering parts of a line (only when rendering
entire lines).

To use, first include this module then import gdl-HTML.xsl using
xsl:import; this will prevent gdl-HTML.xsl's built-in render-o and
render-c functions from being used and the versions in this module
will be used instead.
-->

<xsl:template name="render-o">
  <xsl:variable name="prev-g" 
      select="preceding::g:*[@g:status][1]
	          [ancestor::xtf:l/@xml:id = current()/ancestor::xtf:l/@xml:id]"/>

  <xsl:call-template name="open-breakage">
    <xsl:with-param name="prev-g" select="$prev-g"/>
  </xsl:call-template>
  <xsl:call-template name="open-status">
    <xsl:with-param name="prev-g" select="$prev-g"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="render-c">
  <xsl:variable name="next-g" 
      select="following::g:*[@g:status][1]
	          [ancestor::xtf:l/@xml:id = current()/ancestor::xtf:l/@xml:id]"/>
  <xsl:call-template name="close-breakage">
    <xsl:with-param name="next-g" select="$next-g"/>
  </xsl:call-template>
  <xsl:call-template name="close-status">
    <xsl:with-param name="next-g" select="$next-g"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="open-breakage">
  <xsl:param name="prev-g"/>
  <xsl:choose>
<!--
    <xsl:when test="@g:break='damaged' and not($prev-g[@g:break='damaged'])">
      <xsl:choose>
	<xsl:when test="ancestor::xtf:gloss or @gloss">
	  <xsl:text>&#xB0;</xsl:text>
	</xsl:when>
	<xsl:otherwise>
         <sup><xsl:text>&#xB0;</xsl:text></sup>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
-->
    <xsl:when test="@g:break='missing' 
		    and not($prev-g[@g:break='missing'])">
      <xsl:text>[</xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="open-status">
  <xsl:param name="prev-g"/>
  <xsl:choose>
    <xsl:when test="@g:status='maybe' 
		    and not($prev-g[@g:status='maybe'])">
      <xsl:text>(</xsl:text>
    </xsl:when>
    <xsl:when test="@g:status='supplied' 
		    and not($prev-g[@g:status='supplied'])">
      <xsl:text>&lt;</xsl:text>
    </xsl:when>
    <xsl:when test="@g:status='implied' 
		    and not($prev-g[@g:status='implied'])">
      <xsl:text>&lt;(</xsl:text>
    </xsl:when>
    <xsl:when test="@g:status='excised' 
		    and not($prev-g[@g:status='excised'])">
      <xsl:text>&lt;&lt;</xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="close-status">
  <xsl:param name="next-g"/>
  <xsl:choose>
    <xsl:when test="@g:status='ed.removed' 
		    and not($next-g[@g:status='ed.removed'])">
      <xsl:text>>></xsl:text>
    </xsl:when>
    <xsl:when test="@g:status='scribe.implied' 
		    and not($next-g[@g:status='scribe.implied'])">
      <xsl:text>)></xsl:text>
    </xsl:when>
    <xsl:when test="@g:status='supplied' 
		    and not($next-g[@g:status='supplied'])">
      <xsl:text>></xsl:text>
    </xsl:when>
    <xsl:when test="@g:status='maybe' 
		    and not(next-g[@g:status='maybe'])">
      <xsl:text>)</xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="close-breakage">
  <xsl:param name="next-g"/>
  <xsl:choose>
    <xsl:when test="@g:break='missing' and not($next-g[@g:break='missing'])">
      <xsl:text>]</xsl:text>
    </xsl:when>
<!--
    <xsl:when test="@g:break='damaged' and not($next-g[@g:break='damaged'])">
      <xsl:choose>
	<xsl:when test="ancestor::xtf:gloss or @gloss">
	  <xsl:text>&#xB0;</xsl:text>
	</xsl:when>
	<xsl:otherwise>
         <sup><xsl:text>&#xB0;</xsl:text></sup>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
 -->
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>