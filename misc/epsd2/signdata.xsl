<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		>

<xsl:output method="text" encoding="utf-8"/>

<xsl:include href="gdl-OATF.xsl"/>

<xsl:template match="c:base">
  <xsl:for-each select=".//*[@g:sign][not(ancestor-or-self::g:d)]">
    <xsl:value-of select="@g:sign"/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:choose>
      <xsl:when test="preceding-sibling::*[not(self::g:d)] or ancestor::g:w/preceding-sibling::*">
	<xsl:choose>
	  <xsl:when test="following-sibling::*[not(self::g:d)] or ancestor::g:w/following-sibling::*">
	    <xsl:text>m</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>f</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="following-sibling::*[not(self::g:d)]">
	<xsl:text>i</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:choose>
	  <xsl:when test="count(ancestor::c:text/g:w)>1">
	    <xsl:text>i</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>s</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>&#x9;</xsl:text>
    <xsl:for-each select="ancestor::c:text/g:w">
      <xsl:value-of select="@form"/>
      <xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
    </xsl:for-each>
    <xsl:text>&#x9;</xsl:text>
    <xsl:value-of select="ancestor::c:summary/@n"/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:value-of select="ancestor::c:summary/@ref"/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:value-of select="ancestor::c:base/@icount"/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:value-of select="ancestor::c:base/@ipct"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:for-each>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates select="*"/>
</xsl:template>

<!--<xsl:template match="text()"/>-->

</xsl:stylesheet>
