<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/skl/1.0"
  xmlns:csl="http://oracc.org/ns/signlist/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:gdl="http://oracc.org/ns/gdl/1.0"
  xmlns:s="http://oracc.org/ns/sortkey/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" indent="yes" encoding="utf-8"/>

<xsl:template match="cbd:entries">
    <xsl:text>\begingroup\vocabstyle
</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>\endgroup
</xsl:text>
</xsl:template>

<xsl:template match="cbd:entry">
  <xsl:text>\vocitem{</xsl:text>
  <xsl:choose>
    <xsl:when test="cbd:bases">
      <xsl:for-each select="cbd:bases/cbd:base">
	<xsl:value-of select="."/>
	<xsl:if test="not(position()=last())"><xsl:text>, </xsl:text></xsl:if>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>(</xsl:text>
      <xsl:value-of select="cbd:cf"/>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>}{}{</xsl:text>
  <xsl:choose>
    <xsl:when test="string-length(cbd:pos)=2 and substring(cbd:pos,2)='N'">
    </xsl:when>
    <xsl:otherwise>
      <xsl:text> "</xsl:text>
      <xsl:for-each select="cbd:senses/cbd:sense">
	<xsl:value-of select="cbd:mng"/>
	<xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
      </xsl:for-each>
      <xsl:text>"</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>}{</xsl:text>
  <xsl:value-of select="concat(cbd:cf,' [',cbd:gw,'] ',cbd:pos)"/>
  <xsl:text>}{}
</xsl:text>
</xsl:template>

</xsl:stylesheet>
