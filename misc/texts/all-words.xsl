<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:output method="text" encoding="utf8"/>
  
<xsl:key name="words" match="lex:data" use="*[self::lex:sv or self::lex:wp]/lex:word[1]/@cfgw"/>

<xsl:template match="lex:dataset">
  <lex:word-phra-data>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="word"
			 select="//lex:data[generate-id(.)
				 =generate-id(key('words',*[self::lex:sv or self::lex:wp]/lex:word[1]/@cfgw))]"/>
  </lex:word-phra-data>
</xsl:template>

<xsl:template match="lex:data" mode="word">
  <xsl:text>word&#x9;</xsl:text><xsl:value-of select="*[self::lex:sv or self::lex:wp]/lex:word/@cfgw[1]"/><xsl:text>&#x9;</xsl:text>
  <xsl:choose>
    <xsl:when test="count(*[self::lex:sv or self::lex:wp]/lex:word) > 1">
      <xsl:text>phrase</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>singleton</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="*[self::lex:sv or self::lex:wp]/lex:word/@lang"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>
</xsl:stylesheet>
