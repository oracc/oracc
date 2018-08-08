<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="words" match="lex:data" use="(lex:sv|lex:wp)[1]/lex:word/@cfgw"/>
<xsl:key name="phras" match="lex:data" use="concat(
					    (lex:sv|lex:wp)[1]/lex:word/@cfgw
					    ,':',
					    (lex:sv|lex:wp)[1][starts-with(lex:word[1]/@lang,'sux')]/lex:word/@cfgw
					    )"/>

<xsl:template match="lex:dataset">
  <lex:word-phra-data>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="word"
			 select=".//lex:data[generate-id(.)
				 =generate-id(key('words',
				 (lex:sv|lex:wp)[1]/lex:word/@cfgw))]"/>
  </lex:word-phra-data>
</xsl:template>

<xsl:template match="lex:data" mode="word">
  <xsl:variable name="wd-node" select="(lex:sv|lex:wp)[1]"/>
  <xsl:if test="count($wd-node/*) > 1">
    <xsl:variable name="wd" select="(lex:sv|lex:wp)[1]/lex:word/@cfgw"/>
    <xsl:variable name="ph" select="(lex:sv|lex:wp)[1][starts-with(lex:word[1]/@lang,'sux')]/lex:word/@cfgw"/>
    <lex:group type="word" value="{$wd}">
      <xsl:for-each select="key('words', $wd)[1]">
	<xsl:variable name="nodes" select="key('words', $wd)"/>
	<xsl:apply-templates mode="phra"
			     select="$nodes[generate-id(.)
				     =generate-id(key('phras',
				     concat($wd,':',$ph)))]">
	  <xsl:with-param name="wd" select="$wd"/>
	  <xsl:with-param name="ph" select="$ph"/>
	</xsl:apply-templates>
      </xsl:for-each>
    </lex:group>
  </xsl:if>
</xsl:template>

<xsl:template match="lex:data" mode="phra">
  <xsl:param name="wd"/>
  <xsl:param name="ph"/>
  <xsl:variable name="val">
    <xsl:for-each select="$ph">
      <xsl:value-of select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text> </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:variable>
  <lex:group type="phra" value="{$val}">
    <xsl:attribute name="xml:id"><xsl:value-of select="generate-id()"/></xsl:attribute>
    <xsl:for-each select="key('phra', concat($wd,':',$ph))">
      <xsl:copy>
	<xsl:copy-of select="@*[not(name()='xml:id')]"/>
	<xsl:copy-of select="*"/>
      </xsl:copy>
    </xsl:for-each>
  </lex:group>
</xsl:template>

</xsl:stylesheet>
