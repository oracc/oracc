<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="words" match="lex:data" use="(lex:sv|lex:wp)[1]/lex:word/@cfgw"/>
<xsl:key name="qakks" match="lex:data" use="concat(
					    (lex:sv|lex:wp)[1]/lex:word/@cfgw
					    ,':',
					    lex:eq[starts-with(lex:word[1]/@lang,'akk')][1]/lex:word/@cfgw
					    )"/>

<xsl:template match="lex:dataset">
  <lex:word-qakk-data>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="word"
			 select=".//lex:data[generate-id(.)
				 =generate-id(key('words',
				 (lex:sv|lex:wp)[1]/lex:word/@cfgw))]"/>
  </lex:word-qakk-data>
</xsl:template>

<xsl:template match="lex:data" mode="word">
  <xsl:variable name="wd-node" select="(lex:sv|lex:wp)[1]"/>
  <xsl:if test="count($wd-node/*) = 1">
    <xsl:variable name="wd" select="(lex:sv|lex:wp)[1]/lex:word/@cfgw"/>
    <xsl:variable name="qa" select="lex:eq[starts-with(lex:word[1]/@lang,'akk')][1]/lex:word/@cfgw"/>
    <lex:group type="word" value="{$wd}">
      <xsl:for-each select="key('words', $wd)[1]">
	<xsl:variable name="nodes" select="key('words', $wd)"/>
	<xsl:apply-templates mode="qakk"
			     select="$nodes[generate-id(.)
				     =generate-id(key('qakks',
				     concat($wd,':',$qa)))]">
	  <xsl:with-param name="wd" select="$wd"/>
	  <xsl:with-param name="qa" select="$qa"/>
	</xsl:apply-templates>
      </xsl:for-each>
    </lex:group>
  </xsl:if>
</xsl:template>

<xsl:template match="lex:data" mode="qakk">
  <xsl:param name="wd"/>
  <xsl:param name="qa"/>
  <lex:group type="qakk" value="{$qa}">
    <xsl:attribute name="xml:id"><xsl:value-of select="generate-id()"/></xsl:attribute>
    <xsl:for-each select="key('qakks', concat($wd,':',$qa))">
      <xsl:copy>
	<xsl:copy-of select="@*[not(name()='xml:id')]"/>
	<xsl:copy-of select="*"/>
      </xsl:copy>
    </xsl:for-each>
  </lex:group>
</xsl:template>

</xsl:stylesheet>
