<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		>

<xsl:template match="c:articles|c:letter|c:entry|c:senses">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='xis')]"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="c:cf|c:gw|c:pos">
  <xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="c:sense">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='xis')]"/>
    <xsl:copy-of select="c:pos|c:mng"/>
    <xsl:apply-templates select="c:forms" mode="sense"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="c:forms" mode="sense">
  <xsl:copy>
    <xsl:for-each select="c:form[not(contains(@n,'949'))]">
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:copy-of select="id(@ref)/@stem"/>
	<xsl:copy-of select="id(@ref)/@c"/>
	<xsl:copy-of select="id(@ref)/c:t"/>
	<xsl:copy-of select="c:t"/>
      </xsl:copy>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="*"/>

<xsl:template match="text()"/>

</xsl:stylesheet>
