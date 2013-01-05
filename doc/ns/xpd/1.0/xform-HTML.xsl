<?xml version="1.0" encoding="utf-8"?>
<xsl:transform version="1.0" 
	       xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:xf="http://www.w3.org/2002/xforms"
	       xmlns:ev="http://www.w3.org/2001/xml-events"
	       xmlns:xs="http://www.w3.org/2001/XMLSchema"
	       xmlns:xh="http://www.w3.org/1999/xhtml"
	       xmlns="http://www.w3.org/1999/xhtml" 
	       >

<xsl:template match="xf:model"/>

<xsl:template match="xf:*">
  <xsl:apply-templates select="*"/>
</xsl:template>

<xsl:template match="xh:td[@class='help']">
  <p class="help">
    
  </p>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates select="*|text()"/>
  </xsl:copy>
</xsl:template>

</xsl:transform>