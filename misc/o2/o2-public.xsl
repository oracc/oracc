<?xml version="1.0" encoding="utf-8"?>
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:x="http://oracc.org/ns/xpd/1.0"
	       xmlns="http://oracc.org/ns/xpd/1.0"
	       >

<xsl:param name="public" select="'yes'"/>
<xsl:output method="xml" indent="yes" encoding="utf-8"/>
<xsl:strip-space elements="*"/>

<xsl:template match="x:project">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="nonoption"/>
    <xsl:if test="$public='yes'">
      <public>yes</public>
    </xsl:if>
    <xsl:apply-templates mode="option"/>
  </xsl:copy>
</xsl:template>

<xsl:template mode="option" match="x:option">
  <xsl:copy-of select="."/>
</xsl:template>

<xsl:template mode="option" match="x:*"/>

<xsl:template mode="nonoption" match="x:option|x:public"/>

<xsl:template mode="nonoption" match="x:*">
  <xsl:copy-of select="."/>
</xsl:template>

</xsl:transform>