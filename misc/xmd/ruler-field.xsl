<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xmd="http://oracc.org/ns/xmd/1.0"
		xmlns="http://oracc.org/ns/xmd/1.0"
		exclude-result-prefixes="xmd"
		>

<xsl:template match="xmd:genre[.='Royal Inscription']">
  <xsl:variable name="des" select="normalize-space(../xmd:designation)"/>
  <xsl:variable name="first-name" select="substring-before($des,' ')"/>
  <xsl:variable name="last-names">
    <xsl:if test="string-length($first-name)>0">
      <xsl:value-of select="substring-after($des,' ')"/>
    </xsl:if>
  </xsl:variable>
  <xsl:variable name="ruler">
    <xsl:choose>
      <xsl:when test="contains($last-names, ' ')">
<!--	<xsl:message><xsl:value-of select="substring($last-names,1,1)"/></xsl:message> -->
	<xsl:choose>
	  <xsl:when test="string-length(translate(substring($last-names,1,1),'IVX','')) = 0">
	    <xsl:value-of select="concat($first-name,' ',substring-before($last-names, ' '))"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="$first-name"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="string-length($first-name) > 0">
	<xsl:value-of select="$first-name"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$des"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <ruler>
    <xsl:value-of select="$ruler"/>
  </ruler>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>  
</xsl:template>

<xsl:template match="xmd:*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>