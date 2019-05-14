<?xml version='1.0'?>

<xsl:stylesheet version="1.0"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="no" encoding="utf-8"/>


<xsl:template match="xh:div[@class='tocbanner']">
  <div class="tocbanner">ePSD Sumerian Homographs List</div>
</xsl:template>

<xsl:template match="xh:p[@class='ix']">
  <xsl:if test="count(xh:span[@class='summary'])>1">
    <xsl:copy-of select="."/>
  </xsl:if>
</xsl:template>

<xsl:template match="*|text()">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
