<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0">
<xsl:param name="codesfile" select="'sortcodes.xml'"/>
<xsl:key name="entries" match="sort" use="@key"/>
<xsl:template match="c:entry">
  <xsl:variable name="literal" select="c:cf/@literal"/>
  <xsl:variable name="code">
    <xsl:for-each select="document($codesfile,/)">
      <xsl:value-of select="key('entries',$literal)/@code"/>
    </xsl:for-each>
  </xsl:variable>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="code">
      <xsl:value-of select="$code"/>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>
<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates select="*|text()"/>
  </xsl:copy>
</xsl:template>
</xsl:stylesheet>