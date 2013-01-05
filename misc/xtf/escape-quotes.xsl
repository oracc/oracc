<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="quote-char"><xsl:text>'</xsl:text></xsl:variable>
<xsl:variable name="escaped-quote"><xsl:text>\'</xsl:text></xsl:variable>

<xsl:template name="escape-quotes">
  <xsl:param name="text"/>
  <xsl:choose>
    <xsl:when test="contains($text,$quote-char)">
      <xsl:variable name="pre" select="substring-before($text,$quote-char)"/>
      <xsl:variable name="post">
	<xsl:call-template name="escape-quotes">
	  <xsl:with-param name="text" 
		select="substring-after($text,concat($pre,$quote-char))"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($pre,$escaped-quote,$post)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>