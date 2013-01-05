<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" encoding="utf-8" indent="yes"/>

<xsl:param name="project"/>
<xsl:param name="dateversion"/>
<xsl:param name="abbrev"/>
<xsl:param name="lang"/>

<xsl:template match="xh:head">
  <xsl:copy>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:body">
  <xsl:copy>
    <xsl:attribute name="class">obf</xsl:attribute>
    <div class="obf-header">
      <h1 class="obf-letter">
	<span class="obf-letter"><xsl:value-of select="$abbrev"/> Overview</span>
      </h1>
      <h2 class="obf-letter">
	Version of <xsl:value-of select="$dateversion"/>
      </h2>
    </div>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:a">
<!--
  <xsl:variable name="proj" select="substring-after(substring-before(@href,'&amp;item'),
				                    'project=')"/>
  <xsl:variable name="lang" select="substring-after(@href,'&amp;lang=')"/>

  <xsl:variable name="ID" select="substring-after(substring-before(@href,'&amp;lang'),
				                  'item=')"/>
  <a href="javascript:pop1cbd('/{$proj}/cbd/{$lang}/{$ID}.html')">
    <xsl:apply-templates/>
  </a>
 -->
  <a href="{@href}">
    <xsl:apply-templates/>
  </a>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates select="*|text()"/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
