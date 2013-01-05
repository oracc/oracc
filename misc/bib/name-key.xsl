<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="name-key">
  <xsl:param name="nm" select="text()"/>
  <xsl:choose>
    <xsl:when test="contains($nm,', ')">
      <xsl:variable name="last" select="substring-before($nm,', ')"/>
      <xsl:variable name="first" select="substring-after($nm,', ')"/>
      <xsl:variable name="inits">
	<xsl:call-template name="get-init">
	  <xsl:with-param name="firsts" select="$first"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($last,$inits)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="''"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="get-init">
  <xsl:param name="firsts"/>
  <xsl:value-of select="substring($firsts,1,1)"/>
  <xsl:choose>
    <xsl:when test="contains($firsts,' ')">
      <xsl:call-template name="get-init">
	<xsl:with-param name="firsts" select="substring-after($firsts,' ')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="contains($firsts,'.')">
      <xsl:call-template name="get-init">
	<xsl:with-param name="firsts" select="substring-after($firsts,'.')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>