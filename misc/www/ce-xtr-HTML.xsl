<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:ce="http://oracc.org/ns/ce/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xh xtr">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="ce:ce|ce:data">
  <xsl:copy>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:innerp">
  <xsl:apply-templates/>
  <xsl:if test="following-sibling::xh:innerp">
    <br/><span class="indent"/>
  </xsl:if>
</xsl:template>

<xsl:template match="xh:innerp[@class='tr-comment']"/>

<xsl:template match="xh:span[@class='marker']"/>

<xsl:template match="xh:p[@class='note']"/>

<xsl:template match="xh:*">
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

<xsl:template match="xtr:*"/>

</xsl:stylesheet>
