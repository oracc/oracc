<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://emegir.info/cbd">

<xsl:template match="c:entries">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="c:entry|*/c:entry">
      <xsl:sort data-type="number" select="@code"/>
      <xsl:copy-of select="."/>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>