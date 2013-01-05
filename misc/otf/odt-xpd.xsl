<xsl:stylesheet 
    xmlns:oracc="http://oracc.org/ns/oracc/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"
    >
<xsl:template name="xpd-option">
  <xsl:param name="option"/>
  <xsl:for-each select="/*/office:meta/xpd:project/xpd:option[@name=$option]">
    <xsl:value-of select="@value"/>
  </xsl:for-each>
</xsl:template>
</xsl:stylesheet>