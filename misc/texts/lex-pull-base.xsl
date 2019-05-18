<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<!--		       [count(id(@ref)//g:v[not(@g:status='implied')])>0]
    ]"> -->

<xsl:template match="lex:phrase">
  <xsl:if test="lex:data
		[string-length(@spel)>0 and string-length(lex:sv/@form)>0]"
		>
		<xsl:message>lex:phrase with form <xsl:value-of select="lex:sv/@form"/></xsl:message>
		<xsl:choose>
		  <xsl:when test="count(lex:data/lex:sv/lex:word/*/g:v[not(@g:status='implied')])>0">
		    <xsl:message>lex:implied OK</xsl:message>
		    <xsl:copy-of select="."/>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:message>lex:implied NO</xsl:message>
		  </xsl:otherwise>
		</xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template match="lex:text">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
