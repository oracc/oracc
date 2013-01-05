<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="CMS_A-HTML.xsl"/>

<xsl:template match="/">
  <htmlList>
    <xsl:apply-templates 
	select="t:listBibl/t:biblStruct|*/t:listBibl/t:biblStruct"/>
  </htmlList>
</xsl:template>

<xsl:template match="t:biblStruct">
  <xsl:copy>
    <xsl:copy-of select="@xml:id"/>
    <xsl:call-template name="format-biblStruct"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
