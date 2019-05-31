<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:cbd="http://oracc.org/ns/cbd/1.0"
                version="1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="*">
  <xsl:for-each select="//*[@xml:id or @cbd:id]">
    <xsl:choose>
      <xsl:when test="string-length(@oid)>0">
	<xsl:value-of select="@oid"/>
    <xsl:text>&#xa;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:if test="@xml:id and not(starts-with(@xml:id,'gdl'))">
	  <xsl:value-of select="@xml:id"/>
    <xsl:text>&#xa;</xsl:text>
	</xsl:if>
	<xsl:if test="@cbd:id">
<!--	  <xsl:message>cbd:id = <xsl:value-of select="@cbd:id"/></xsl:message> -->
	  <xsl:value-of select="@cbd:id"/>
    <xsl:text>&#xa;</xsl:text>
	</xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
