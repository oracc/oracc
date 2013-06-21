<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" encoding="utf-8" indent="yes"/>
<xsl:strip-space elements="*"/>

<xsl:param name="tmpdir"/>
<xsl:param name="results" select="concat($tmpdir,'/results.xml')"/>

<xsl:variable name="jsStart">
  <xsl:text>javascript:oasResults('</xsl:text>
</xsl:variable>
<xsl:variable name="jsMid">
  <xsl:text>','</xsl:text>
</xsl:variable>
<xsl:variable name="jsEnd">
  <xsl:text>')</xsl:text>
</xsl:variable>

<xsl:template match="result">
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
	<xsl:attribute name="list">@TMPDIR@/results.txt</xsl:attribute>
	<notify>
	  <xsl:value-of select="concat($jsStart,'@TMPDIR@',$jsEnd)"/>
	</notify>
      </xsl:when>
      <xsl:otherwise>
	<xsl:copy-of select="@*"/>
      </xsl:otherwise>
    </xsl:choose>
    <toks>
      <xi:include xmlns:xi="http://www.w3.org/2001/XInclude" href="{$tmpdir}/search.txt" parse="text"/>
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
