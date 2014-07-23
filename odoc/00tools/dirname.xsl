<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns="http://www.w3.org/1999/xhtml" 
		xmlns:d="http://oracc.org/ns/xdf/1.0"
		xmlns:dc="http://purl.org/dc/elements/1.1"
		xmlns:dcterms="http://purl.org/dc/terms/"
		xmlns:h="http://www.w3.org/1999/xhtml" 
   		xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
		version="1.0"
		exclude-result-prefixes="d dc dcterms xsi">

<xsl:template name="dirname">
  <xsl:param name="f" select="'linganno/AKK/akkstyle/akkstyle.xdf'"/>
  <xsl:variable name="last-slash">
    <xsl:call-template name="rindex">
      <xsl:with-param name="str" select="$f"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:value-of select="substring($f,1,
			string-length($f)
			- (string-length($last-slash)+1))"/>
</xsl:template>

<xsl:template name="rindex">
  <xsl:param name="str"/>
  <xsl:param name="chr" select="'/'"/>
  <xsl:choose>
    <xsl:when test="not(contains($str,$chr))">
      <xsl:value-of select="$str"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="rindex">
	<xsl:with-param name="str" select="substring-after($str,$chr)"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
