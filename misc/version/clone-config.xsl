<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns="http://oracc.org/ns/xpd/1.0"
		xmlns:xpd="http://oracc.org/ns/xpd/1.0"
		>

<xsl:output method="xml" encoding="utf-8" indent="yes"/>

<xsl:variable name="clone-config" 
	      select="document('../01tmp/config.xml',/)/*"/>

<xsl:template match="xpd:project">
  <xsl:copy>
    <xsl:copy-of select="$clone-config/@n"/>
    <xsl:copy-of select="@*[not(local-name(.)='n')]"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xpd:abbrev">
  <xsl:copy-of select="$clone-config/xpd:abbrev"/>
</xsl:template>

<xsl:template match="xpd:logo">
  <xsl:copy-of select="$clone-config/xpd:logo"/>
</xsl:template>

<xsl:template match="xpd:name">
  <xsl:copy-of select="$clone-config/xpd:name"/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
