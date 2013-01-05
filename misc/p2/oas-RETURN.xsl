<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" encoding="utf-8" indent="yes"/>
<xsl:strip-space elements="*"/>

<xsl:param name="results"/>
<xsl:param name="search"/>

<xsl:variable name="jsNotify">
  <xsl:text>javascript:oasNotify('</xsl:text>
</xsl:variable>

<xsl:variable name="jsOutline">
  <xsl:text>javascript:oasOutline('</xsl:text>
</xsl:variable>

<xsl:variable name="jsResults">
  <xsl:text>javascript:oasResults('</xsl:text>
</xsl:variable>

<xsl:variable name="jsMid">
  <xsl:text>','</xsl:text>
</xsl:variable>
<xsl:variable name="jsEnd">
  <xsl:text>')</xsl:text>
</xsl:variable>

<xsl:template match="result">
  <xsl:variable name="list" select="document($results,/)/*/dir"/>
  <!--<xsl:message><xsl:value-of select="/*/post-type"/></xsl:message>-->
  <result>
    <xsl:choose>
      <xsl:when test="/search or /browse">
	<xsl:attribute name="count">
	  <xsl:choose>
	    <xsl:when test="document($results,/)/*/count">
	      <xsl:value-of select="document($results,/)/*/count"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text>0</xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:attribute>
	<xsl:attribute name="list">
	  <xsl:value-of select="$list"/>
	</xsl:attribute>
	<notify>
	  <xsl:value-of select="concat($jsNotify,
				$list,
				$jsEnd)"/>
	</notify>
      </xsl:when>
      <xsl:otherwise>
	<xsl:copy-of select="@*"/>
      </xsl:otherwise>
    </xsl:choose>
    <toks>
      <xsl:value-of select="$search"/>
    </toks>
  </result>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
