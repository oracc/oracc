<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/skl/1.0"
  xmlns:csl="http://oracc.org/ns/signlist/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:gdl="http://oracc.org/ns/gdl/1.0"
  xmlns:s="http://oracc.org/ns/sortkey/1.0"
  xmlns:skl="http://oracc.org/ns/skl/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" indent="yes" encoding="utf-8"/>

<xsl:template match="csl:signlist">
  <xsl:text>\begingroup\signliststyle
</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>\endgroup
</xsl:text>
</xsl:template>

<xsl:template match="csl:sign">
  <xsl:text>\SLsign{</xsl:text>
  <xsl:value-of select="csl:utf8"/>
  <xsl:text>}{</xsl:text>
  <xsl:value-of select="@n"/>
  <xsl:text>}{}{</xsl:text>
  <xsl:for-each select="csl:v">
    <xsl:value-of select="."/>
    <xsl:if test="not(position()=last())"><xsl:text>, </xsl:text></xsl:if>
  </xsl:for-each>
  <xsl:text>}
</xsl:text>
</xsl:template>

</xsl:stylesheet>
