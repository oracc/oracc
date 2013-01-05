<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:cbd="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns="http://oracc.org/ns/cbd/1.0"
		>

<xsl:output method="xml" encoding="utf-8"/>

<xsl:template name="s">
  <s>
    <xsl:for-each select="g:w">
      <xsl:for-each select="./*[@g:sign]">
	<xsl:value-of select="@g:sign"/>
	<xsl:if test="not(position()=last())">
	  <xsl:text>.</xsl:text>
	</xsl:if>
      </xsl:for-each>
      <xsl:if test="not(position()=last())">
	<xsl:text> </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </s>
</xsl:template>

<xsl:template match="cbd:t">
  <xsl:call-template name="s"/>
  <xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates select="*|text()"/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>