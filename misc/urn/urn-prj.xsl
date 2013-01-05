<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:xpd="http://oracc.org/ns/xpd/1.0">
<xsl:output method="text" encoding="utf-8"/>
<xsl:variable name="prefix" select="'urn:oracc:prj:'"/>

<xsl:template match="projects">
  <xsl:for-each select="xpd:project">
    <xsl:sort select="@n"/>
    <xsl:apply-templates select="."/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xpd:project">
  <xsl:value-of select="concat($prefix,@n,'&#x9;', xpd:name, '&#xa;')"/>
</xsl:template>

</xsl:transform>
