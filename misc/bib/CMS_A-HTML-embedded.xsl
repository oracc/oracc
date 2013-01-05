<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="CMS_A-HTML.xsl"/>
<xsl:include href="ckey-namedate.xsl"/>

<xsl:template match="t:listBibl">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="t:biblStruct">
  <xsl:call-template name="format-biblStruct"/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
