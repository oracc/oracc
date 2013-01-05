<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:gdl="http://oracc.org/ns/gdl/1.0">

<xsl:output method="text" encoding="utf8"/>

<xsl:key name="utf8s" match="*[@gdl:utf8]" use="@gdl:utf8"/>

<xsl:template match="gdl:*">
  <xsl:if test="@gdl:utf8">
    <xsl:value-of select="@gdl:utf8"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>