<xsl:stylesheet 
    xmlns:oracc="http://oracc.org/ns/oracc/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    >

<xsl:output method="xml" indent="no" omit-xml-declaration="yes"/>

<xsl:template match="xpd:option">
  <xsl:element name="option">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="opt" select="*|text()"/>
  </xsl:element>
  <xsl:text>
</xsl:text>
</xsl:template>

<xsl:template mode="opt" match="*">
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="opt" select="*|text()"/>
  </xsl:element>
</xsl:template>

<xsl:template mode="opt" match="text()">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
