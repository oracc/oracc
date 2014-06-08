<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://oracc.org/ns/xis/1.0"
  xmlns:xis="http://oracc.org/ns/xis/1.0">

<xsl:param name="sub"/>

<xsl:template match="xis:xisses">
  <xsl:copy>
    <xsl:copy-of select="*"/>
    <xsl:for-each select="document($sub, /)/*">
      <xsl:copy-of select="*"/>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
