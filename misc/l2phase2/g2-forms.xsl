<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:i="http://oracc.org/ns/instances/1.0"
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:note="http://oracc.org/ns/note/1.0"
  xmlns:n="http://oracc.org/ns/norm/1.0"
  xmlns:norm="http://oracc.org/ns/norm/1.0"
  xmlns:usg="http://oracc.org/ns/usg/1.0"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="dc i xl xcl norm usg note">

<xsl:key name="norms" match="cbd:norm/cbd:forms/cbd:f" use="@ref"/>

<xsl:template match="/">
  <formsfile>
    <xsl:copy-of select="*/@n|*/@project|*/@xml:lang"/>
    <xsl:apply-templates/>
  </formsfile>
</xsl:template>

<xsl:template match="cbd:entry">
  <forms>
    <xsl:copy-of select="@n|@xis|@icount|@ipct"/>
    <xsl:attribute name="xml:id"><xsl:value-of select="concat(@xml:id,'.f')"/></xsl:attribute>
    <xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
    <xsl:for-each select="cbd:forms/cbd:form">
      <xsl:copy>
	<xsl:copy-of select="@n|@icount|@ipct|@xis"/>
	<xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
	<xsl:apply-templates/>
	<norms>
	  <xsl:for-each select="key('norms',@xml:id)">
	    <xsl:for-each select="ancestor::cbd:norm">
	      <xsl:copy>
		<xsl:copy-of select="@icount|@ipct|@xis"/>
		<xsl:attribute name="n"><xsl:value-of select="cbd:n"/></xsl:attribute>
	      </xsl:copy>
	    </xsl:for-each>
	  </xsl:for-each>
	</norms>
      </xsl:copy>
    </xsl:for-each>
  </forms>
</xsl:template>

<xsl:template match="cbd:articles|cbd:letter">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
