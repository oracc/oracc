<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" indent="no" encoding="utf-8"/>

<xsl:template match="g:v|g:s|g:r">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:b">
  <xsl:choose>
    <xsl:when test="g:o">
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:q">
  <xsl:apply-templates select="*[1]"/>
  <xsl:text>(</xsl:text>
  <xsl:apply-templates select="*[2]"/>
  <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="g:a">
  <xsl:text>~</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:m">
  <xsl:text>@</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:n">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:for-each select="g:b">
        <xsl:apply-templates select="g:r"/>
        <xsl:text>(</xsl:text>
        <xsl:apply-templates select="*[2]"/>
	<xsl:apply-templates select="g:a|g:m"/>
        <xsl:text>)</xsl:text>
      </xsl:for-each>
     </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates select="g:r"/>
      <xsl:text>(</xsl:text>
      <xsl:apply-templates select="*[2]"/>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:apply-templates select="g:a|g:m"/>
</xsl:template>

<xsl:template match="g:c">
  <xsl:text>|</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>|</xsl:text>
</xsl:template>

<xsl:template match="g:g">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:o">
  <xsl:choose>
    <xsl:when test="@type='beside'"><xsl:text>.</xsl:text></xsl:when>
    <xsl:when test="@type='joining'"><xsl:text>+</xsl:text></xsl:when>
    <xsl:when test="@type='containing'"><xsl:text>×</xsl:text></xsl:when>
    <xsl:when test="@type='above'"><xsl:text>&amp;</xsl:text></xsl:when>
    <xsl:when test="@type='crossing'"><xsl:text>%</xsl:text></xsl:when>
    <xsl:when test="@type='opposing'"><xsl:text>@</xsl:text></xsl:when>
    <xsl:when test="@type='repeated'">
      <xsl:value-of select="concat(.,'x')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>gdl-ATF: <xsl:value-of select="@type"/> not handled</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:*">
  <xsl:message>gdl-ATF: <xsl:value-of select="name()"/> not handled</xsl:message>
  <xsl:apply-templates/>
</xsl:template>

</xsl:stylesheet>
