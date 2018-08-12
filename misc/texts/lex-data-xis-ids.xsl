<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns="http://oracc.org/ns/xis/1.0"
		xmlns:xis="http://oracc.org/ns/xis/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="lex xi">

<xsl:param name="idbase" select="'word'"/>
  
<xsl:key name="xisses" match="lex:group" use="@xis"/>

<xsl:template match="/">
  <xisses>
    <xsl:for-each select=".//lex:group[generate-id(.)=generate-id(key('xisses',@xis))]">
      <xis>
	<xsl:attribute name="xml:id">
	  <xsl:value-of select="concat($idbase,'.xis.',position())"/>
	</xsl:attribute>
	<xsl:attribute name="efreq">
	  <xsl:value-of select="count(.//lex:data)"/>
	</xsl:attribute>
	<r><xsl:value-of select="@xis"/></r>
      </xis>
    </xsl:for-each>
  </xisses>
</xsl:template>

</xsl:stylesheet>
