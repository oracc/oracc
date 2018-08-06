<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lsi="http://oracc.org/ns/lsi/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:key name="signs" match="lsi:data" use="@sign"/>
<xsl:key name="reads" match="lsi:data" use="@read"/>
<xsl:key name="spels"  match="lsi:data" use="concat(@read,':',@spel)"/>

<xsl:template match="lsi:data-set|lsi:text">
  <lsi:sign-data>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="sign"
	select="lsi:data[generate-id(.)=generate-id(key('signs',@sign)[1])]"/>
  </lsi:sign-data>
</xsl:template>

<xsl:template match="lsi:data" mode="sign">
  <lsi:group type="sign" value="{@sign}">
    <xsl:for-each select="key('signs', @sign)[1]">
      <xsl:variable name="nodes" select="key('signs', @sign)"/>
      <xsl:apply-templates mode="read"
			   select="$nodes[generate-id(.)=generate-id(key('reads',@read)[1])]"/>	
    </xsl:for-each>
  </lsi:group>
</xsl:template>

<xsl:template match="lsi:data" mode="read">
  <lsi:group type="read" value="{@read}">
    <xsl:for-each select="key('reads', @read)[1]">
      <xsl:variable name="nodes" select="key('reads', @read)"/>
      <xsl:apply-templates mode="spel"
			   select="$nodes[generate-id(.)=generate-id(key('spels',
				   concat(@read,':',@spel))[1])]"/>	
    </xsl:for-each>
  </lsi:group>
</xsl:template>

<xsl:template match="lsi:data" mode="spel">
  <lsi:group type="spel" value="{@spel}">
    <xsl:for-each select="key('spels', concat(@read,':',@spel))">
      <xsl:copy-of select="."/>
    </xsl:for-each>
  </lsi:group>
</xsl:template>

</xsl:stylesheet>
