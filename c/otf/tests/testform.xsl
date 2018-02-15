<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:g="http://oracc.org/ns/gdl/1.0"
	       version="1.0">

<xsl:output method="text" encoding="UTF-8"/>
<xsl:template match="g:w">
  <xsl:value-of select="@form"/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
