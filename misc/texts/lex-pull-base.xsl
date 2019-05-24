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
  <xsl:variable name="use-data"
		select="lex:data
			[string-length(@spel)>0 and (string-length(lex:sv/@form)>0 or string-length(@read)>0)]"/>

<!--<xsl:message>lex:phrase <xsl:value-of select="@phrase"/> has <xsl:value-of select="count($use-data)"/> members</xsl:message>-->
			
  <xsl:if test="count($use-data) > 0">
    <xsl:copy-of select="."/>
    <!--
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="$use-data"/>
    </xsl:copy>
    -->
  </xsl:if>
</xsl:template>

<xsl:template match="lex:datax">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="*">
      <xsl:choose>
	<xsl:when test="self::lex:sv">
	  <xsl:if test="count(lex:data/lex:sv/lex:word//g:v[not(@g:status='implied')])>0">
	    <xsl:copy>
	      <xsl:copy-of select="@*"/>
	      <xsl:copy-of select="lex:word//g:v[not(@g:status='implied')]"/>
	    </xsl:copy>
	  </xsl:if>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:copy-of select="."/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:copy>
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
