<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:template match="lex:phrase">
  <xsl:for-each select=".//lex:word[string-length(@oid)>0][position()>1]">
    <xsl:variable name="cfgw" select="@cfgw"/>
    <xsl:variable name="oid" select="@oid"/>
    <xsl:for-each select="ancestor::lex:phrase">      
      <xsl:copy>
	<xsl:attribute name="head">
	  <xsl:value-of select="$cfgw"/>
	</xsl:attribute>
	<xsl:attribute name="oid">
	  <xsl:value-of select="$oid"/>
	</xsl:attribute>
	<xsl:copy-of select="@*[not(name()='head') and not(name()='oid') and not(name()='xml:id')]"/>
	<xsl:if test="@xml:id">
	  <xsl:attribute name="xml-id"><xsl:value-of select="@xml:id"/></xsl:attribute>
	</xsl:if>
	<xsl:apply-templates select="*"/>
      </xsl:copy>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

<xsl:template match="lex:text">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='xml:id')]"/>
    <xsl:if test="@xml:id">
      <xsl:attribute name="xml-id"><xsl:value-of select="@xml:id"/></xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
