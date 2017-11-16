<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xmd="http://oracc.org/ns/xmd/1.0"
		xmlns="http://www.w3.org/1999/xhtml"
		exclude-result-prefixes="xmd"
		>

<xsl:import href="html-standard.xsl"/>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:variable name="title">
    <xsl:text>Catalogue summaries for </xsl:text>
    <xsl:value-of select="*/@project"/>
  </xsl:variable>
  <xsl:call-template name="make-html">
    <xsl:with-param name="title" select="$title"/>
    <xsl:with-param name="project" select="*/@project"/>
    <xsl:with-param name="webtype" select="'cbd'"/>
    <xsl:with-param name="with-hr" select="false()"/>
    <xsl:with-param name="with-trailer" select="false()"/>    
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xmd:summary">
  <p id="{@xml:id}">
    <xsl:value-of select="concat(/*/@project,':',xmd:id_text, ' = ', xmd:designation, '. ')"/>
    <xsl:if test="string-length(xmd:period)"><xsl:value-of select="concat(xmd:period, ' ')"/></xsl:if>
    <xsl:if test="string-length(xmd:provenience)"><xsl:value-of select="concat(xmd:provenience, ' ')"/></xsl:if>
    <xsl:if test="string-length(xmd:genre) or string-length(xmd:subgenre)">
      <xsl:text>(</xsl:text>
      <xsl:if test="string-length(xmd:genre)"><xsl:value-of select="xmd:genre"/></xsl:if>
      <xsl:if test="string-length(xmd:genre) and string-length(xmd:subgenre)"><xsl:text>/</xsl:text>></xsl:if>
      <xsl:if test="string-length(xmd:subgenre)"><xsl:value-of select="xmd:subgenre"/></xsl:if>
      <xsl:text>)</xsl:text>
    </xsl:if>
  </p>
</xsl:template>

</xsl:stylesheet>
