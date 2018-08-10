<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://oracc.org/ns/xis/1.0"
  xmlns:xis="http://oracc.org/ns/xis/1.0">

<xsl:output method="text"/>
  
<xsl:template match="xis:xis">
  <xsl:value-of select="@xml:id"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="@efreq"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="xis:r/text()"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

</xsl:stylesheet>
