<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:oxpd="http://emegir.info/xpd"
		xmlns="http://oracc.org/ns/xpd/1.0"
		xmlns:xpd="http://oracc.org/ns/xpd/1.0"
		exclude-result-prefixes="oxpd"
		>

<xsl:output method="xml" encoding="utf-8" indent="yes"/>
<xsl:strip-space elements="*"/>
<xsl:template name="coerce">
  <xsl:param name="e" select="local-name()"/>
  <xsl:element name="{$e}">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

<xsl:template match="oxpd:options">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="oxpd:project">
  <project n="{oxpd:abbreviations/oxpd:basename}">
    <xsl:apply-templates select="oxpd:name"/>
    <abbrev><xsl:value-of select="oxpd:abbreviations/oxpd:standard"/></abbrev>
    <xsl:apply-templates select="*[not(self::oxpd:name)]"/>
  </project>
</xsl:template>

<xsl:template match="oxpd:abbreviations|oxpd:corpus|oxpd:atf-sources"/>

<xsl:template match="oxpd:*">
  <xsl:call-template name="coerce"/>
</xsl:template>

</xsl:stylesheet>
