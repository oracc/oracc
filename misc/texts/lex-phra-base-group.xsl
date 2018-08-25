<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="words" match="lex:phrase" use="concat(@lang,':',@head)"/>
<xsl:key name="bases" match="lex:phrase" use="concat(@lang,':',@head,':',
					      lex:data[1]/@read)"/>
<xsl:key name="spels" match="lex:phrase" use="concat(@lang,':',@head,':',
					      lex:data[1]/@read,
					      lex:data[1]/@spel)"/>

<xsl:template match="lex:phrases">
  <lex:phra-base-data>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="word"
			 select="//lex:phrase[generate-id(.)
				 =generate-id(key('words',concat(@lang,':',@head)))]"/>
  </lex:phra-base-data>
</xsl:template>

<xsl:template match="lex:phrase[@lang='sux']" mode="word">
  <lex:group type="word" value="{@head}" oid="{@oid}">
    <xsl:variable name="nodes" select="key('words',concat(@lang,':',@head))"/>
    <xsl:for-each select="$nodes[generate-id(.)
			  =generate-id(key('bases',concat(@lang,':',@head,':',
			  lex:data[1]/@read)))]">
      <xsl:apply-templates select="." mode="base"/>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:phrase[@lang='sux']" mode="base">
  <lex:group type="base" lang="{@lang}" value="{lex:data[1]/@read}">
    <xsl:variable name="nodes" select="key('bases',concat(@lang,':',@head,':',
			  lex:data[1]/@read))"/>
    <xsl:for-each select="$nodes[generate-id(.)
			  =generate-id(key('spels',concat(@lang,':',@head,':',
					      lex:data[1]/@read,
					      lex:data[1]/@spel)))]">
      <xsl:apply-templates select="." mode="spels"/>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:phrase" mode="spels">
  <lex:group type="spel" value="{lex:data[1]/@spel}">
    <xsl:attribute name="xml:id"><xsl:value-of select="generate-id()"/></xsl:attribute>
    <xsl:for-each select="key('spels',concat(@lang,':',@head,':',
					      lex:data[1]/@read,
					      lex:data[1]/@spel))">
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
