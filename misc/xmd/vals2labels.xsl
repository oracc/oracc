<?xml version='1.0' encoding='utf-8'?>
<!-- @SUMMARY@= convert sortvals.xml to sortlabels.xml, a flat hash table-->
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="sortvals">
  <sortlabels>
    <xsl:apply-templates/>
  </sortlabels>
</xsl:template>

<xsl:template match="genres">
  <xsl:apply-templates>
    <xsl:with-param name="prefix" select="'G'"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="names">
  <xsl:apply-templates>
    <xsl:with-param name="prefix" select="'N'"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="periods">
  <xsl:apply-templates>
    <xsl:with-param name="prefix" select="'T'"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="places">
  <xsl:apply-templates>
    <xsl:with-param name="prefix" select="'P'"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="supergenres">
  <xsl:apply-templates>
    <xsl:with-param name="prefix" select="'S'"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="v">
  <xsl:param name="prefix"/>
  <xsl:if test="not(@r = '0')">
    <v k="{@n}">
      <xsl:attribute name="xml:id">
        <xsl:value-of select="concat($prefix,@c)"/>
      </xsl:attribute>
    </v>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
