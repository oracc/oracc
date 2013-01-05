<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    version="1.0">

<xsl:output method="text" encoding="utf8"/>

<xsl:template match="xpd:image">
  <xsl:value-of select="concat(ancestor::xpd:project/@n, ':', ., '&#xa;')"/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
