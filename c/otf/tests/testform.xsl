<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:g="http://oracc.org/ns/gdl/1.0"
	       xmlns:n="http://oracc.org/ns/norm/1.0"
	       version="1.0">

<xsl:output method="text" encoding="UTF-8"/>

<xsl:template match="/">
  <xsl:variable name="w" select=".//g:w|.//n:w"/>
  <xsl:for-each select="$w[1]">
    <xsl:value-of select="@form"/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
