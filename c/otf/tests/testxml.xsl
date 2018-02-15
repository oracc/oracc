<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:g="http://oracc.org/ns/gdl/1.0"
	       version="1.0"
	       exclude-result-prefixes="g">

<xsl:template match="g:w">
  <xsl:copy>
    <xsl:apply-templates mode="copy-elements"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="text()"/>

<xsl:template match="*" mode="copy-elements">
  <xsl:copy>
    <xsl:copy-of select="@g:delim"/>
    <xsl:copy-of select="@g:logolang"/>
    <xsl:copy-of select="@g:pos"/>
    <xsl:copy-of select="@g:role"/>
    <xsl:copy-of select="@g:type"/>
    <xsl:apply-templates mode="copy-elements"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="text()" mode="copy-elements">
  <xsl:value-of select="."/>
</xsl:template>

</xsl:transform>
