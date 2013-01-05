<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:xh="http://www.w3.org/1999/xhtml"
		xmlns:xforms="http://www.w3.org/2002/xforms"
		xmlns:xs="http://www.w3.org/2001/XMLSchema"
		xmlns:ev="http://www.w3.org/2001/xml-events"
		exclude-result-prefixes="xh xforms xs ev"
		>

<xsl:strip-space elements="*"/>

<xsl:param name="instance" select="'pager'"/>

<xsl:output method="xml" omit-xml-declaration="yes"/>

<xsl:template match="/">
  <xsl:apply-templates mode="echo" 
		       select=".//xforms:instance[@id=$instance]/*"/>
</xsl:template>

<xsl:template mode="echo" match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="echo"/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
