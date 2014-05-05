<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet version="1.0" 
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  exclude-result-prefixes="xmd xst xtr xh">

<xsl:output method="xml" encoding="utf-8" indent="yes" omit-xml-declaration="yes"/>

<xsl:param name="frag-id"/>
<xsl:param name="line-id"/>
<xsl:param name="project" select="''"/>

<xsl:variable name="q">'</xsl:variable>

<xsl:template match="xh:body">
  <xsl:variable name="hash" select="id($line-id)/*/*/*/@name"/>
  <xsl:variable name="onload">
    <xsl:if test="string-length($hash) > 0">
      <xsl:value-of select="concat('window.location.hash=', $q,$hash,$q)"/>
    </xsl:if>
  </xsl:variable>
  <xsl:copy>
    <xsl:copy-of select="@class"/>
    <xsl:if test="string-length($onload)>0">
      <xsl:attribute name="onload">
	<xsl:value-of select="concat($onload,'; ', 'p2Keys()')"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:table">
  <xsl:variable name="id" select="@xml:id"/>
  <xsl:variable name="select">
    <xsl:if test="$id=$line-id">
      <xsl:message>hiliting <xsl:value-of select="$line-id"/></xsl:message>
      <xsl:text> selected</xsl:text>
    </xsl:if>
  </xsl:variable>
  <xsl:variable name="class" select="concat(@class, $select)"/>
  <xsl:copy>
    <xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
    <xsl:copy-of select="*"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
