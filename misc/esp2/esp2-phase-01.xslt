<?xml version="1.0" encoding="utf-8"?>

<!-- esp-phase-01.xslt operates on structure.xml  -->

<xsl:stylesheet
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xpath-default-namespace="http://www.w3.org/1999/xhtml"
    version="2.0" 
    >

  <xsl:output 
      method="xml" 
      encoding="utf-8"
      indent="no"
      />

  <xsl:param name="oracc"/>
  <xsl:param name="scripts"/>
  <xsl:param name="project"/>
  <xsl:param name="projesp"/>
  <xsl:param name="output-file"/>

  <xsl:template match="/">
    <!--	<xsl:message>Phase 1: Resolve and filter source tree</xsl:message> -->
    <xsl:result-document href="{$output-file}"> 
      <xsl:apply-templates/>
    </xsl:result-document>
  </xsl:template>

  <!-- add content of individual page files to source tree -->
  <xsl:template match="struct:page[@file]">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:variable name="page" 
		    select="document ( concat ( $projesp, '/00web/', @file ) )"/>
      <xsl:if test="not ( string ( $page ) )">
	<xsl:message>	WARNING! Page file '<xsl:value-of select="@file"/>' not found.</xsl:message>
      </xsl:if>
      <xsl:if test="not($page/esp:page)">
	<xsl:message>   WARNING! Page file '<xsl:value-of select="@file"/>' must begin with 'page' element in 'http://oracc.org/ns/esp/1.0' namespace.</xsl:message>
      </xsl:if>
      <xsl:apply-templates select="$page/esp:page/node ()"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="struct:link">
    <struct:page>
      <xsl:copy-of select="@*"/>
      <xsl:attribute name="type">link</xsl:attribute>
      <esp:name><xsl:value-of select="@title"/></esp:name>
      <esp:title><xsl:value-of select="@title"/></esp:title>
    </struct:page>
  </xsl:template>

  <!-- copy recognised tags unchanged -->
  <xsl:template match="esp:* | html | head | body | meta | p | pre
		       | ol | ul | li | em | b | i | q | blockquote 
		       | abbr | acronym | br | h2 | h3 | code | kbd 
		       | table | th | tr | td | thead | tbody | caption | tfoot
		       | dl | dd | dt | strong | style | sup | sub | address 
		       | object | param | embed | div | script | small | iframe
		       | span | form | input | select | hr | audio | source">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <!-- strip unrecognised tags and warn -->
  <xsl:template match="*">
    <xsl:message>	esp2 WARNING! Unrecognised element '<xsl:value-of select="name ()"/>' was filtered.</xsl:message>
    <xsl:apply-templates/>
  </xsl:template>

</xsl:stylesheet>
