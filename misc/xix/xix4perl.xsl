<xsl:stylesheet version="1.0" 
    xmlns:xix="http://emegir.info/index"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"/>

<xsl:template match="/">
  <xsl:for-each select="//xix:where">
    <xsl:value-of select="substring(/*/@basename,1,2)"/>
    <xsl:text>:</xsl:text>
    <xsl:value-of select="@ref"/>
    <xsl:text>:</xsl:text>
    <xsl:value-of select="../@sortkey"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>