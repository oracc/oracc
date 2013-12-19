<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.oracc.org/ns/cbd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="issl">
  <entries>
    <xsl:apply-templates/>
  </entries>
</xsl:template>

<xsl:template match="entry">
  <entry>
    <xsl:copy-of select="@xml:id"/>
    <xsl:apply-templates select="term"/>
  </entry>
</xsl:template>

<xsl:template match="term">
  <form n="{.}"/>
</xsl:template>

</xsl:stylesheet>
