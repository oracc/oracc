<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="words" match="lex:phrase" use="concat(@lang,':',@head)"/>
<xsl:key name="phras" match="lex:phrase" use="concat(@lang,':',@head,':',@phrase)"/>
<xsl:key name="equis" match="lex:phrase" use="concat(@head,':',@phrase,':',@equiv)"/>
<xsl:key name="lines" match="lex:phrase" use="concat(@head,':',@phrase,':',lex:data/@line)"/>

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
    <xsl:variable name="head" select="@head"/>
    <xsl:variable name="nodes" select="key('phras',concat(@lang,':',@head,':',@phrase))"/>
<!--
    <xsl:if test="contains($head,'NU[')">
      <xsl:variable name="p" select="concat(@lang,':',@head,':',@phrase)"/>
      <xsl:variable name="c" select="count(key('phras',$p))"/>
      <xsl:message>lex:phrase key=<xsl:value-of select="$p"/>; count=<xsl:value-of select="$c"/></xsl:message>
      <xsl:for-each select="$nodes">
	<xsl:message><xsl:value-of select="concat(@head,':',@phrase,':',@equiv)"
	/>; id(.)=<xsl:value-of select="generate-id(.)"
	/>; id(key)=<xsl:value-of select="generate-id(key('equis',concat(@head,':',@phrase,':',@equiv)))"/></xsl:message>
      </xsl:for-each>
      </xsl:if>
-->
    <xsl:for-each select="$nodes[generate-id(.)
			  =generate-id(key('equis',concat(@head,':',@phrase,':',@equiv)))]">
      <xsl:apply-templates select="." mode="equi"/>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:phrase[@lang='sux']" mode="equi">
  <lex:group type="equi" lang="{@lang}" value="{@equiv}">
    <xsl:variable name="nodes" select="key('equis',concat(@head,':',@phrase,':',@equiv))"/>
    <xsl:for-each select="$nodes[generate-id(.)
			  =generate-id(key('lines',concat(@head,':',@phrase,':',lex:data/@line)))]">
      <xsl:apply-templates select="." mode="line"/>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:phrase" mode="line">
  <lex:group type="line" value="{lex:data/@line}">
    <xsl:attribute name="xml:id"><xsl:value-of select="generate-id()"/></xsl:attribute>
    <xsl:for-each select="key('lines',concat(@head,':',@phrase,':',lex:data/@line))">
      <xsl:for-each select="*">
	<xsl:copy>
	  <xsl:copy-of select="@*[not(name()='xml:id')]"/>
	  <xsl:copy-of select="*"/>
	</xsl:copy>
      </xsl:for-each>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:phrase" mode="word"/>

<xsl:template match="*" mode="word">
  <xsl:message>warning: unhandled node <xsl:value-of select="name()"/> while mode=word</xsl:message>
</xsl:template>

<xsl:template match="*" mode="phra">
  <xsl:message>warning: unhandled node <xsl:value-of select="name()"/> while mode=phra</xsl:message>
</xsl:template>

<xsl:template match="*" mode="equi">
  <xsl:message>warning: unhandled node <xsl:value-of select="name()"/> while mode=equi</xsl:message>
</xsl:template>

<xsl:template match="*" mode="line">
  <xsl:message>warning: unhandled node <xsl:value-of select="name()"/> while mode=line</xsl:message>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
