<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		>

<xsl:output method="text" encoding="UTF-8"/>
  
<xsl:template match="*[@xml:id and @n]">
  <xsl:value-of select="concat(@xml:id,'&#x9;',@n,'&#xa;')"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
