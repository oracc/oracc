<?xml version='1.0'?>

<!--

 XSL Stylesheet to generate round-tripped ATF version of 
 CDLI texts, i.e., a version which should be (modulo
 unimportant changes) the same as the original ATF format.

 v1.0.  Placed in the Public Domain.

 -->

<xsl:stylesheet version="1.0" 
  xmlns:c="http://cdli.ucla.edu/text/1"
  xmlns:d="http://psd.museum.upenn.edu/debug/1"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:strip-space elements="*"/>
<xsl:output method="text" indent="yes"/>

<xsl:template match="c:text">
  <xsl:text>&amp;</xsl:text>
  <xsl:value-of select="@n"/>
  <xsl:text>&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="c:object">
  <xsl:text>&#xa;</xsl:text>
  <xsl:text>@</xsl:text>
  <xsl:choose>
    <xsl:when test="@type='other'">
      <xsl:text>object </xsl:text>
      <xsl:value-of select="@object"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@type"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="certain"/>
  <xsl:text>&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="c:surface">
  <xsl:text>&#xa;</xsl:text>
  <xsl:text>@</xsl:text>
  <xsl:choose>
    <xsl:when test="@type='obverse'">
      <xsl:text>obverse</xsl:text>
    </xsl:when>
    <xsl:when test="@type='reverse'">
      <xsl:text>reverse</xsl:text>
    </xsl:when>
    <xsl:when test="@type='surface'">
      <xsl:text>surface </xsl:text>
      <xsl:value-of select="@surface"/>
    </xsl:when>
    <xsl:when test="@type='left'">
      <xsl:text>left</xsl:text>
    </xsl:when>
    <xsl:when test="@type='right'">
      <xsl:text>right</xsl:text>
    </xsl:when>
    <xsl:when test="@type='top'">
      <xsl:text>top</xsl:text>
    </xsl:when>
    <xsl:when test="@type='bottom'">
      <xsl:text>bottom</xsl:text>
    </xsl:when>
    <xsl:when test="@type='edge'">
      <xsl:text>edge</xsl:text>
    </xsl:when>
    <xsl:when test="@type='face'">
      <xsl:text>face </xsl:text>
      <xsl:value-of select="@face"/>
    </xsl:when>
    <xsl:otherwise>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="certain"/>
  <xsl:text>&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="c:sealing">
  <xsl:text>&#xa;</xsl:text>
  <xsl:text>@seal </xsl:text>
  <xsl:value-of select="@n"/>
  <xsl:call-template name="certain"/>
  <xsl:text>&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="c:noncolumn|c:nonl">
  <xsl:text>$ </xsl:text>
  <xsl:value-of select="text()"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="c:cmt">
  <xsl:text>#</xsl:text>
  <xsl:value-of select="text()"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="c:column">
  <xsl:if test="not(@n = '0')">
    <xsl:text>@column </xsl:text>
    <xsl:value-of select="@o"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:if>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="c:l">
  <xsl:value-of select="@o"/>
  <xsl:text>. </xsl:text>
  <xsl:value-of select="text()"/>
  <xsl:apply-templates select="c:atf"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="certain">
  <xsl:if test="@certain = 'n'">
    <xsl:text>?</xsl:text>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
