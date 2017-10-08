<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:note="http://oracc.org/ns/note/1.0"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  exclude-result-prefixes="xmd xst xtr xh">

<xsl:include href="p3-corpusview-lib.xsl"/>

<xsl:output method="xml" encoding="utf-8" indent="yes" omit-xml-declaration="yes"/>

<xsl:param name="frag-id"/>
<xsl:param name="host"/>
<xsl:param name="line-id"/>
<xsl:param name="project" select="''"/>
<xsl:param name="trans" select="'en'"/>
<xsl:param name="transonly" select="'false'"/>
<xsl:param name="txhdir" select="''"/>

<xsl:variable name="q">'</xsl:variable>

<xsl:template match="/">
  <xsl:call-template name="corpusview-project-pqid">
    <xsl:with-param name="project" select="/*/@project"/> <!-- $project -->
    <xsl:with-param name="host" select="$host"/>
    <xsl:with-param name="pqid" select="/*/@xml:id"/>
    <xsl:with-param name="txhdir" select="$txhdir"/>
  </xsl:call-template>  
</xsl:template>

<xsl:template match="xtf:include">
  <xsl:variable name="iproject" select="substring-before(@ref, ':')"/>
  <xsl:variable name="ipqid" select="substring-after(@ref, ':')"/>
<!--  <xsl:message>xtf:include: n=<xsl:value-of select="@n"/>; iproject=<xsl:value-of select="$iproject"/>; ipqid=<xsl:value-of select="$ipqid"/></xsl:message>-->
<!--  <h2><xsl:value-of select="@n"/></h2> -->
  <xsl:call-template name="corpusview-project-pqid">
    <xsl:with-param name="project" select="$iproject"/>
    <xsl:with-param name="pqid" select="$ipqid"/>
  </xsl:call-template>    
</xsl:template>

</xsl:stylesheet>
