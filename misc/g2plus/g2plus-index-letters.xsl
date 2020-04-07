<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/xix/1.0"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xix="http://oracc.org/ns/xix/1.0"
  xmlns:ex="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="ex">

<xsl:output method="xml" encoding="utf-8" indent="no"/>

<xsl:param name="base"/>

<xsl:key name="toc" match="xix:ix" 
	 use="substring(translate(@letterkey,
	      'ABCDEFGHIJKLMNOPQRSTUVWXYZŠŊṢṬŚ0123456789',
	      'abcdefghijklmnopqrstuvwxyzšŋṣṭś0000000000'),1,1)"/>

<xsl:template name="initials">
  <i l="a" u="A"/>
  <i l="b" u="B"/>
  <i l="c" u="C"/>
  <i l="d" u="D"/>
  <i l="e" u="E"/>
  <i l="f" u="F"/>
  <i l="g" u="G"/>
  <i l="ŋ" u="Ŋ" fss="NG"/>
  <i l="h" u="H"/>
  <i l="i" u="I"/>
  <i l="j" u="J"/>
  <i l="k" u="K"/>
  <i l="l" u="L"/>
  <i l="m" u="M"/>
  <i l="n" u="N"/>
  <i l="o" u="O"/>
  <i l="p" u="P"/>
  <i l="q" u="Q"/>
  <i l="r" u="R"/>
  <i l="s" u="S"/>
  <i l="š" u="Š" fss="SH"/>
  <i l="ṣ" u="Ṣ" fss="SS"/>
  <i l="ś" u="Ś" fss="SA"/>
  <i l="t" u="T"/>
  <i l="ṭ" u="Ṭ" fss="TT"/>
  <i l="u" u="U"/>
  <i l="v" u="V"/>
  <i l="w" u="W"/>
  <i l="x" u="X"/>
  <i l="y" u="Y"/>
  <i l="z" u="Z"/>
  <i l="0" u="0" title="Numbers" fss="0"/>
</xsl:template>

<xsl:template match="xix:index">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:variable name="index" select="."/>
    <xsl:for-each select="document('')/*/xsl:template[@name='initials']/xix:i">
      <xsl:variable name="i" select="@l"/>
      <xsl:variable name="u" select="@u"/>
      <xsl:variable name="c">
        <xsl:choose>
          <xsl:when test="@title"><xsl:value-of select="@title"/></xsl:when>
          <xsl:otherwise><xsl:value-of select="@u"/></xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <xsl:for-each select="$index">
	<xsl:variable name="ixnodes" select="key('toc',$i)"/>
	<xsl:message><xsl:value-of select="count($ixnodes)"/> ixnodes for TOC entry <xsl:value-of select="$i"/> under letter <xsl:value-of select="$c"/></xsl:message>
	<xsl:if test="count($ixnodes) > 0">
	  <letter><xsl:value-of select="$c"/></letter>
	  <xsl:for-each select="$ixnodes">
  	    <xsl:copy-of select="."/>
	  </xsl:for-each>
	</xsl:if>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
