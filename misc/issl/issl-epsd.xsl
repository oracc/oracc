<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" indent="no" encoding="utf-8"/>

<xsl:template match="cbd:entry">
  <xsl:variable name="e" select="."/>
  <xsl:variable name="issl">
    <xsl:if test="cbd:bib"><xsl:text>*</xsl:text></xsl:if>
  </xsl:variable>
  <xsl:for-each select="cbd:bases/cbd:base/@n">
    <xsl:value-of select="."/>
    <xsl:text> :: :: :: ePSD :: </xsl:text>
    <xsl:value-of select="concat($issl,$e/@xml:id,' ',
				 $e/cbd:cf,'[',$e/cbd:gw,']')"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:for-each>
</xsl:template>

<xsl:template match="cbd:sup">
  <xsl:value-of select="concat('{',.,'}')"/>
</xsl:template>

</xsl:stylesheet>
