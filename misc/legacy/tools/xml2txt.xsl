<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="1.0" >

<xsl:output method="text"/>

<xsl:template match="note">
  <xsl:text>^</xsl:text><xsl:value-of select="note-citation"/><xsl:text>^{{</xsl:text>
  <xsl:apply-templates select="body"/>
  <xsl:text>}}</xsl:text>
</xsl:template>

<xsl:template match="p">
  <xsl:if test="string-length(.) > 0">
    <xsl:apply-templates/>
    <xsl:text>&#x0a;&#x0a;</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="span">
  <xsl:variable name="tag">
    <xsl:call-template name="decode-style"/>
  </xsl:variable>
  <xsl:if test="string-length(@super) > 0">
    <xsl:text>{</xsl:text>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="string-length($tag)">
      <xsl:value-of select="concat('@',$tag,'{')"/>
      <xsl:apply-templates/>
      <xsl:value-of select="concat('}',$tag,'@')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="string-length(@super) > 0">
    <xsl:text>}</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="tab">
  <xsl:text>&#x9;</xsl:text>
</xsl:template>

<xsl:template match="s">
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template name="decode-style">
  <xsl:choose>
    <xsl:when test="contains(@style, ' b ')">
      <xsl:text>b</xsl:text>
    </xsl:when>
    <xsl:when test="contains(@style, ' i ')">
      <xsl:text>i</xsl:text>
    </xsl:when>
    <xsl:otherwise>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>