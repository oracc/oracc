<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:cbd="http://oracc.org/ns/cbd/1.0"
		xmlns="http://oracc.org/ns/cbd/1.0"
		>

<!--
    Note 1: this script does not presently create a sig header; this
    is OK as long as these sigs are only used as lemmer data sources.

    Note 2: this script does not need to worry about non-merged .g2c
    files (i.e., those which have no @ok on sense); these files are
    linked to glossary.sig by g2-sig.plx
-->

<xsl:output method="text" encoding="utf-8"/>

<xsl:variable name="epos-quote"><xsl:text>'</xsl:text></xsl:variable>

<xsl:template match="cbd:sense[@ok='1']">
  <xsl:for-each select="cbd:sigs/cbd:sig[not(cbd:cof-data) or cbd:cof-data/cbd:cof-head/@curr='yes']">
    <xsl:choose>
      <xsl:when test="starts-with(@sig, '{')">
	<xsl:call-template name="start-psu"/>
	<xsl:call-template name="dump">
	  <xsl:with-param name="sig" select="substring-after(@sig,'{')"/>
	</xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="dump"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>

<xsl:template name="dump">
  <xsl:param name="sig" select="@sig"/>
  <xsl:value-of select="$sig"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="@icount"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="@ipct"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="@xis"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="id(@xis-periods)/@key"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="start-psu">
  <xsl:text>{</xsl:text>
  <xsl:choose>
    <xsl:when test="@ngram">
      <xsl:value-of select="@ngram"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:for-each select="ancestor::cbd:entry/cbd:compound/cbd:cpd">
	<xsl:value-of select="concat(cbd:cf,'[',cbd:gw,'//',cbd:sense,']',cbd:pos,$epos-quote,cbd:epos)"/>
	<xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
      </xsl:for-each>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text> += </xsl:text>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
