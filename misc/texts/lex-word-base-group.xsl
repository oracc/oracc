<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="words" match="lex:data" use="(lex:sv[1]/lex:word/@cfgw)|(lex:wp[1]/lex:word/@cfgw)"/>
<xsl:key name="bases" match="lex:data" use="concat(
					    (lex:sv[1]/lex:word/@cfgw)|(lex:wp[1]/lex:word/@cfgw)
					    ,':',
					    (lex:sv[1]/lex:word/@base)|(lex:wp[1]/lex:word/@base)
					    )"/>
<xsl:key name="wdbas" match="lex:word" use="concat(@cfgw,':',@base)"/>
<xsl:key name="spels" match="lex:data" use="concat((lex:sv[1]/lex:word/@cfgw)|(lex:wp[1]/lex:word/@cfgw),
					            ':',
					            (lex:sv[1]/lex:word/@base)|(lex:wp[1]/lex:word/@base),
						    ':',
						    @spel
						    )"/>
<!--<xsl:key name="spels"  match="lex:data" use="concat(@sign,':',@read,':',@spel)"/>-->

<xsl:template match="lex:dataset">
  <lex:word-base-data>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="word"
			 select=".//lex:data[generate-id(.)
				 =generate-id(key('words',
				 (lex:sv[1]/lex:word/@cfgw)|(lex:wp[1]/lex:word/@cfgw)))]"/>
  </lex:word-base-data>
</xsl:template>

<xsl:template match="lex:data" mode="word">
  <xsl:variable name="wd" select="(lex:sv|lex:wp)[1]/lex:word/@cfgw"/>
  <xsl:variable name="bs" select="(lex:sv|lex:wp)[1]/lex:word/@base"/>
  <lex:group type="word" value="{$wd}">
    <xsl:for-each select="key('words', $wd)[1]">
      <xsl:variable name="nodes" select="key('words', $wd)"/>
      <xsl:apply-templates mode="base"
			   select="$nodes[generate-id(.)
				          =generate-id(key('bases',
					                   concat($wd,':',$bs)))]"/>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:data" mode="base">
  <xsl:variable name="wd" select="(lex:sv|lex:wp)[1]/lex:word/@cfgw"/>
  <xsl:variable name="bs" select="(lex:sv|lex:wp)[1]/lex:word/@base"/>
  <xsl:variable name="wdbs" select="concat($wd,':',$bs)"/>
  <lex:group type="base" value="{$bs}">
    <xsl:for-each select="key('bases', $wdbs)[1]">
      <xsl:variable name="nodes" select="key('bases', $wdbs)"/>
      <xsl:apply-templates mode="spel"
			   select="$nodes[generate-id(.)
				          =generate-id(key('spels',
						           concat($wdbs,':',@spel)))]"/>
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:data" mode="spel">
  <xsl:variable name="wd" select="(lex:sv|lex:wp)[1]/lex:word/@cfgw"/>
  <xsl:variable name="bs" select="(lex:sv|lex:wp)[1]/lex:word/@base"/>
  <xsl:variable name="wdbs" select="concat($wd,':',$bs)"/>
  <lex:group type="spel" value="{@spel}">
    <xsl:attribute name="xml:id"><xsl:value-of select="generate-id()"/></xsl:attribute>
    <xsl:for-each select="key('spels', concat($wdbs,':',@spel))">
      <xsl:copy>
	<xsl:copy-of select="@*[not(name()='xml:id')]"/>
      </xsl:copy>
    </xsl:for-each>
  </lex:group>
</xsl:template>

</xsl:stylesheet>
