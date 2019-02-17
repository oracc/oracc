<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		>

<xsl:output method="text" encoding="utf-8"/>

<xsl:include href="gdl-OATF.xsl"/>

<xsl:template match="c:base">
  <xsl:for-each select=".//*[@g:sign]">
    <xsl:value-of select="@g:sign"/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:choose>
      <xsl:when test="preceding-sibling::*">
	<xsl:choose>
	  <xsl:when test="following-sibling::*">
	    <xsl:text>m</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>f</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="following-sibling::*">
	<xsl:text>i</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>s</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>&#x9;</xsl:text>
    <xsl:value-of select="ancestor::g:w/@form"/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:value-of select="ancestor::c:summary/@n"/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:value-of select="ancestor::c:summary/@ref"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:for-each>
</xsl:template>
<xsl:template match="text()"/>
</xsl:stylesheet>
