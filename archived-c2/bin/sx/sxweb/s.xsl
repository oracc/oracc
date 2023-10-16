<xsl:transform xmlns:g="http://oracc.org/gdl/1.0"
	       xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       version="1.0">

  <xsl:template match="g:w">
    <xsl:value-of select="concat(@xml:id,'&#x9;',@form,'&#xa;')"/>
  </xsl:template>
  
</xsl:transform>
