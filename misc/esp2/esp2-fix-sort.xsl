<?xml version="1.0" encoding="utf-8"?>
<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:xh="http://www.w3.org/1999/xhtml">
<xsl:template match="xh:div[@id='Glossary']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="xh:div[*[1][not(@id='letter__')]]"/>
    <xsl:copy-of select="xh:div[*[1][@id='letter__']]"/>
  </xsl:copy>
</xsl:template>
<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates select="*|text()"/>
  </xsl:copy>
</xsl:template>
</xsl:transform>
