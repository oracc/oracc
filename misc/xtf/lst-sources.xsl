<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       >

<xsl:output method="text"/>
  
<xsl:template match="refs[@type='sources']">
  <xsl:if test="count(*)>0">
    <xsl:value-of select="concat(@ref,'&#xa;')"/>
  </xsl:if>
</xsl:template>

</xsl:transform>
