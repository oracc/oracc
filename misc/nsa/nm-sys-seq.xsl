<?xml version='1.0'?>

<xsl:transform version="1.0" 
  xmlns:nm="http://psd.museum.upenn.edu/nm" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:strip-space elements="*"/>

<xsl:output method="text"/>

<xsl:template match="nm:system">
  <xsl:value-of select="@n"/>
  <xsl:text>-</xsl:text>
  <xsl:value-of select="@type"/>
  <xsl:text>-</xsl:text>
  <xsl:value-of select="@time"/>
  <xsl:text>-</xsl:text>
  <xsl:value-of select="@place"/>
  <xsl:text>: </xsl:text>
  <xsl:for-each select="nm:step">
    <xsl:value-of select="@atf"/>
      <xsl:if test="not(position()=last())">
	<xsl:text>, </xsl:text>
      </xsl:if>
  </xsl:for-each>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

</xsl:transform>
