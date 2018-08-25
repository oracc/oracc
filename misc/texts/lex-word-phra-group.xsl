<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="words" match="lex:phrase" use="concat(@lang,':',@head)"/>
<xsl:key name="phras" match="lex:phrase" use="concat(@lang,':',@head,':',@phrase)"/>
<xsl:key name="equis" match="lex:phrase" use="concat(@phrase,':',@equiv)"/>
<xsl:key name="lines" match="lex:phrase" use="concat(@phrase,':',lex:data/@line)"/>

<xsl:template match="lex:phrases">
  <lex:word-phra-data>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="word"
			 select="//lex:phrase[generate-id(.)
				 =generate-id(key('words',concat(@lang,':',@head)))]"/>
  </lex:word-phra-data>
</xsl:template>

<xsl:template match="lex:phrase[@lang='sux']" mode="word">
  <lex:group type="word" value="{@head}">
    <xsl:copy-of select="@oid"/>
    <xsl:variable name="nodes" select="key('words',concat(@lang,':',@head))"/>
    <xsl:for-each select="$nodes[generate-id(.)
			  =generate-id(key('phras',concat(@lang,':',@head,':',@phrase)))]">
      <xsl:apply-templates select="." mode="phra"/>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:phrase[@lang='sux']" mode="phra">
  <lex:group type="phra" lang="{@lang}" value="{@phrase}">
    <xsl:variable name="nodes" select="key('phras',concat(@lang,':',@head,':',@phrase))"/>
    <xsl:for-each select="$nodes[generate-id(.)
			  =generate-id(key('equis',concat(@phrase,':',@equiv)))]">
      <xsl:apply-templates select="." mode="equi"/>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:phrase[@lang='sux']" mode="equi">
  <lex:group type="equi" lang="{@lang}" value="{@equiv}">
    <xsl:variable name="nodes" select="key('equis',concat(@phrase,':',@equiv))"/>
    <xsl:for-each select="$nodes[generate-id(.)
			  =generate-id(key('lines',concat(@phrase,':',lex:data/@line)))]">
      <xsl:apply-templates select="." mode="line"/>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:phrase" mode="line">
  <lex:group type="line" value="{lex:data/@line}">
    <xsl:attribute name="xml:id"><xsl:value-of select="generate-id()"/></xsl:attribute>
    <xsl:for-each select="key('lines',concat(@phrase,':',lex:data/@line))">
      <xsl:for-each select="*">
	<xsl:copy>
	  <xsl:copy-of select="@*[not(name()='xml:id')]"/>
	  <xsl:copy-of select="*"/>
	</xsl:copy>
      </xsl:for-each>
    </xsl:for-each>
  </lex:group>
</xsl:template>

</xsl:stylesheet>
