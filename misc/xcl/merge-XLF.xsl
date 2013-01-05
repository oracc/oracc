<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/xcl/1.0"
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:lem="http://oracc.org/ns/lemma/1.0"
  xmlns:syn="http://oracc.org/ns/syntax/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xcl lem">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:param name="xlf-file" select="concat(/*/@ref,'.xlf')"/>
<xsl:variable name="xlf" select="document($xlf-file,/)/*"/>

<xsl:key name="wid" match="lem:w" use="@wid"/>

<xsl:template match="xcl:w">
  <xsl:variable name="ref" select="@ref"/>
  <xsl:variable name="scp" select="@syn:ub-after"/>
  <xsl:variable name="pre-syn" select="@syn:brk-before"/>
  <xsl:variable name="post-syn" select="@syn:brk-after"/>
  <xsl:for-each select="$xlf">
    <xsl:variable name="f-nodes" select="key('wid',$ref)/*"/>
    <xsl:choose>
      <xsl:when test="count($f-nodes) > 1">
	<ll>
          <xsl:apply-templates select="$f-nodes">
            <xsl:with-param name="ref" select="$ref"/>
            <xsl:with-param name="scp" select="$scp"/>
            <xsl:with-param name="pre-syn" select="$pre-syn"/>
            <xsl:with-param name="post-syn" select="$post-syn"/>
          </xsl:apply-templates>
	</ll>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="$f-nodes">
          <xsl:with-param name="ref" select="$ref"/>
          <xsl:with-param name="scp" select="$scp"/>
	  <xsl:with-param name="pre-syn" select="$pre-syn"/>
	  <xsl:with-param name="post-syn" select="$post-syn"/>
        </xsl:apply-templates>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>

<xsl:template match="lem:f">
  <xsl:param name="ref"/>
  <xsl:param name="scp"/>
  <xsl:param name="pre-syn"/>
  <xsl:param name="post-syn"/>
  <l ref="{$ref}">
    <xsl:if test="string-length($scp) > 0">
      <xsl:attribute name="scp"><xsl:value-of select="$scp"/></xsl:attribute>
    </xsl:if>
    <xsl:if test="string-length($pre-syn) > 0">
      <xsl:attribute name="pre-syn"><xsl:value-of select="$pre-syn"/></xsl:attribute>
    </xsl:if>
    <xsl:if test="string-length($post-syn) > 0">
      <xsl:attribute name="post-syn"><xsl:value-of select="$post-syn"/></xsl:attribute>
    </xsl:if>
    <xsl:copy-of select="@*"/>
  </l>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
