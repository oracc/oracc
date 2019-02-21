<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:ex="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:o="http://oracc.org/ns/oracc/1.0"
  exclude-result-prefixes="xh"
  extension-element-prefixes="ex"
>

<xsl:import href="html-standard.xsl"/>

<xsl:param name="lang" select="'sux'"/>
<xsl:param name="type" select="'wp'"/>
<xsl:param name="webdir" select="'01tmp/lex'"/>

<xsl:variable name="subdir-str">
  <xsl:choose>
    <xsl:when test="string-length($lang) > 0">
      <xsl:value-of select="concat($lang,'/')"/>
    </xsl:when>
    <xsl:otherwise/>
  </xsl:choose>
</xsl:variable>

<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>

<xsl:template match="/">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xh:body">
  <xsl:for-each select="*[string-length(@o:id)>0]">
    <xsl:call-template name="output"/>
  </xsl:for-each>
</xsl:template>

<xsl:template name="output">
  <xsl:variable name="outfile" select="concat('./',$webdir,'/cbd/',$subdir-str,$type,'/',@o:id,'.html')"/>
<!--  <xsl:message>gsplit div[class=body] outfile=<xsl:value-of select="$outfile"/></xsl:message> -->
  <ex:document href="{$outfile}"
	     omit-xml-declaration="yes"
	     method="xml"
	     encoding="utf-8"
	     indent="yes">
    <xsl:copy-of select="."/>
  </ex:document>
</xsl:template>

</xsl:stylesheet>
