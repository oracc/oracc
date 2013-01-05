<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.tei-c.org/ns/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:strip-space elements="*"/>
<xsl:output method="xml" indent="yes"/>

<xsl:template match="list">
  <listBibl>
    <xsl:apply-templates/>
  </listBibl>
</xsl:template>

<xsl:template match="item">
  <xsl:variable name="id" select="concat('b',10000+position())"/>
  <biblStruct id="{$id}">
    <xsl:attribute name="type">
      <xsl:choose>
  	<xsl:when test="T and B">
          <xsl:text>chapter</xsl:text>
        </xsl:when>
  	<xsl:when test="T and J">
          <xsl:text>article</xsl:text>
        </xsl:when>
  	<xsl:when test="B or T">
          <xsl:text>book</xsl:text>
        </xsl:when>
  	<xsl:when test="J">
          <xsl:text>article</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:message>ref2tei.xsl: can't assign type to <xsl:value-of 
                       select="$id"/></xsl:message>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  <analytic>
    <xsl:apply-templates select="A"/>
    <xsl:apply-templates select="T"/>
  </analytic>
  <monogr>
    <xsl:choose>
      <xsl:when test="J">
        <xsl:apply-templates select="J"/>
      </xsl:when>
      <xsl:when test="B">
        <xsl:apply-templates select="B"/>
        <xsl:apply-templates select="E"/>
      </xsl:when>
    </xsl:choose>
    <imprint>
      <xsl:if test="not(S)">
        <xsl:apply-templates select="V"/>
      </xsl:if>
      <xsl:apply-templates select="P"/>
      <xsl:apply-templates select="I"/>
      <xsl:apply-templates select="C"/>
      <xsl:apply-templates select="D"/>
    </imprint>
  </monogr>
  <xsl:if test="S">
    <series>
      <xsl:apply-templates select="S"/>
      <xsl:apply-templates select="V|N"/>
    </series>
  </xsl:if>
  <xsl:apply-templates select="K"/>
  <xsl:apply-templates select="F"/>
  <xsl:apply-templates select="O"/>
  </biblStruct>
</xsl:template>

<xsl:template match="A">
  <author><xsl:copy-of select="*|text()"/></author>
</xsl:template>

<xsl:template match="E">
  <editor><xsl:copy-of select="*|text()"/></editor>
</xsl:template>

<xsl:template match="T|B|J|S">
  <title><xsl:copy-of select="*|text()"/></title>
</xsl:template>

<xsl:template match="P">
  <biblScope type="pages"><xsl:apply-templates/></biblScope>
</xsl:template>

<xsl:template match="V">
  <xsl:choose>
    <xsl:when test="../S">
      <biblScope type="number"><xsl:apply-templates/></biblScope>
    </xsl:when>
    <xsl:otherwise>
      <biblScope type="vol"><xsl:apply-templates/></biblScope>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="N">
  <biblScope type="number"><xsl:apply-templates/></biblScope>
</xsl:template>

<xsl:template match="I">
  <publisher><xsl:apply-templates/></publisher>
</xsl:template>

<xsl:template match="C">
  <pubPlace><xsl:apply-templates/></pubPlace>
</xsl:template>

<xsl:template match="D">
  <date><xsl:apply-templates/></date>
</xsl:template>

<xsl:template match="K">
  <note type="keys"><xsl:apply-templates/></note>
</xsl:template>

<xsl:template match="F">
  <note type="abbrev"><xsl:apply-templates/></note>
</xsl:template>

<xsl:template match="O">
  <note><xsl:apply-templates/></note>
</xsl:template>

</xsl:stylesheet>
