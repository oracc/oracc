<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="signs" match="lex:data" use="@sign"/>
<xsl:key name="reads" match="lex:data" use="concat(@sign,':',@read)"/>
<xsl:key name="spels" match="lex:data" use="concat(@sign,':',@read,':',@spel)"/>

<xsl:template match="lex:dataset">
  <lex:sign-data>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="sign"
	select=".//lex:data[generate-id(.)=generate-id(key('signs',@sign)[1])]"/>
  </lex:sign-data>
</xsl:template>

<xsl:template match="lex:data" mode="sign">
  <lex:group type="sign" value="{@sign}">
    <xsl:for-each select="key('signs', @sign)[1]">
      <xsl:variable name="nodes" select="key('signs', @sign)"/>
      <xsl:apply-templates mode="read"
			   select="$nodes[generate-id(.)=generate-id(key('reads',
				   concat(@sign,':',@read))[1])]"/>	
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:data" mode="read">
  <lex:group type="read" value="{@read}">
    <xsl:for-each select="key('reads', concat(@sign,':',@read))[1]">
      <xsl:variable name="nodes" select="key('reads', concat(@sign,':',@read))"/>
      <xsl:apply-templates mode="spel"
			   select="$nodes[generate-id(.)=generate-id(key('spels',
				   concat(@sign,':',@read,':',@spel))[1])]"/>	
    </xsl:for-each>
  </lex:group>
</xsl:template>

<xsl:template match="lex:data" mode="spel">
  <lex:group type="spel" value="{@spel}">
    <xsl:attribute name="xml:id"><xsl:value-of select="generate-id()"/></xsl:attribute>
    <xsl:for-each select="key('spels', concat(@sign,':',@read,':',@spel))">
      <xsl:copy>
	<xsl:copy-of select="@*"/>
      </xsl:copy>
    </xsl:for-each>
  </lex:group>
</xsl:template>

</xsl:stylesheet>
