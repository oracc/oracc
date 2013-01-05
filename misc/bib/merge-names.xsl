<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
		version="1.0">
<xsl:output mode="xml" indent="yes" encoding="utf-8"/>

<xsl:variable name="merge-doc" select="/"/>

<xsl:key name="merge-keys" match="name" use="@key"/>

<xsl:template match="/">
  <xsl:apply-templates 
      mode="merge"
      select="document('/usr/local//lib/bib/names.xml')"/>
</xsl:template>

<xsl:template mode="merge" match="namemap">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="merge" select="*"/>
    <xsl:copy-of select="$merge-doc//name[@type='none']"/>
  </xsl:copy>
</xsl:template>

<xsl:template mode="merge" match="name">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="*"/>
    <xsl:variable name="k" select="@key"/>
    <xsl:for-each select="$merge-doc">
      <xsl:for-each select="key('merge-keys',$k)">
	<xsl:copy-of select="aka"/>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="*"/>

<xsl:template match="text()"/>

</xsl:stylesheet>