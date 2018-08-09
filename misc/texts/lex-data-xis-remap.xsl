<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xis="http://oracc.org/ns/xis/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="lex">

<xsl:key name="xis" match="xis:xis" use="xis:r"/>

<xsl:template match="lex:group">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='xis')]"/>
    <xsl:variable name="xis" select="@xis"/>
    <xsl:attribute name="xis">
      <xsl:for-each select="document('xis.xml',/)">
	<xsl:value-of select="key('xis',$xis)/@xml:id"/>
      </xsl:for-each>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
