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

<xsl:template match="xh:head"/>

<xsl:template match="xh:body">
  <xsl:apply-templates select="xh:div[@class='lex-word'][string-length(@o:id)>0]"/>
</xsl:template>

<!--
    doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
    -->

<xsl:template match="xh:div[@class='lex-word']">
  <xsl:variable name="outfile" select="concat('./',$webdir,'/cbd/',$subdir-str,$type,'/',@o:id,'.html')"/>
<!--  <xsl:message>gsplit div[class=body] outfile=<xsl:value-of select="$outfile"/></xsl:message> -->
<ex:document href="{$outfile}"
	     omit-xml-declaration="yes"
	     method="xml"
	     encoding="utf-8"
	     indent="yes">
    <xsl:copy-of select="."/>
    <!--
	 <xsl:call-template name="make-html">
	 <xsl:with-param name="project" select="$project"/>
	 <xsl:with-param name="webtype" select="'cbd'"/>
	 <xsl:with-param name="with-hr" select="false()"/>
	 <xsl:with-param name="with-trailer" select="false()"/>
	 <xsl:with-param name="body-class" select="ancestor::xh:body/@class"/>
	 </xsl:call-template>
    -->
  </ex:document>
</xsl:template>

<!--
<xsl:template name="call-back">
  <xsl:copy-of select="*"/>
</xsl:template>
-->

</xsl:stylesheet>
