<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xis="http://oracc.org/ns/xis/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:epad="http://psd.museum.upenn.edu/epad/"
  exclude-result-prefixes="epad">

<xsl:param name="projectDir"/>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="cbd:entries">
  <summaries>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </summaries> 
</xsl:template>

<xsl:template match="cbd:letter">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="cbd:entry">
  <summary>
    <xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
<!--    <xsl:copy-of select="@*[not(self::xml:id)]|cbd:overview/@periods"/> -->
    <xsl:for-each select="@*">
      <xsl:if test="not(local-name(.)='id')">
	<xsl:copy-of select="."/>
      </xsl:if>
    </xsl:for-each>
    <xsl:attribute name="periods">
      <xsl:variable name="pid" select="concat(@xis,'.p')"/>
      <xsl:variable name="periods-xis" select="concat($projectDir,'/01bld/', /*/@xml:lang,'/periods.xis')"/>
<!--      <xsl:message>periods-xis=<xsl:value-of select="$periods-xis"/></xsl:message> -->
      <xsl:for-each select="document($periods-xis,/)">
	<xsl:for-each select="id($pid)/xis:p">
	  <xsl:value-of select="."/>
	  <xsl:if test="not(position()=last())">
	    <xsl:text>, </xsl:text>
	  </xsl:if>
	</xsl:for-each>
      </xsl:for-each>
    </xsl:attribute>
    <xsl:apply-templates 
	select="cbd:cf|cbd:gw|cbd:pos|cbd:root|cbd:dt|./*/cbd:base|./cbd:senses/cbd:sense/cbd:mng|./*/*/cbd:term"/>
  </summary>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
