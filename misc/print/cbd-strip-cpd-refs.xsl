<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://emegir.info/cbd">

<xsl:key name="cpdrefs" 
	 match="c:r[ancestor::c:entry[@compound='yes']]" 
	 use="@ref"/>

<xsl:template match="c:entry[@compound='yes']">
  <xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="c:r">
  <xsl:if test="count(key('cpdrefs',@ref))=0">
    <xsl:copy-of select="."/>
  </xsl:if>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>