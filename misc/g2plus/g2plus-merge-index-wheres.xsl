<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns:xix="http://oracc.org/ns/xix/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="no" encoding="utf-8"/>
<!--<xsl:key name="what" match="xix:what" use="../@sortkey"/>-->
<xsl:key name="what" match="xix:what" use="."/>

<xsl:template match="xix:index">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xix:ix">
  <xsl:variable name="what" select="xix:what"/>
  <xsl:if test="generate-id(xix:what) = generate-id(key('what',$what))">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:copy-of select="xix:what"/>
      <xsl:for-each select="key('what',xix:what)">
	<xsl:sort select="translate(following-sibling::*/text(),
			  'ŋhištu','hijtuv')"/>
	<xsl:copy-of select="following-sibling::*"/>
      </xsl:for-each>
    </xsl:copy>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
