<?xml version="1.0" encoding="utf-8"?>

<!-- esp-phase-01.xsl operates on structure.xml  -->

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:h="http://www.w3.org/1999/xhtml"
    xmlns:ex="http://exslt.org/common"
    xmlns:eb="http://oracc.org/ns/esp-biblatex/1.0"
    extension-element-prefixes="ex"
    version="1.0" 
    >

  <xsl:output  method="xml" encoding="utf-8" indent="no"/>

  <xsl:param name="oracc"/>
  <xsl:param name="scripts"/>
  <xsl:param name="project"/>
  <xsl:param name="projesp"/>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- add content of individual page files to source tree -->
  <xsl:template match="struct:page[@file]">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:variable name="pagename" select="concat ( $projesp, '/00web/', @file )"/>
      <xsl:variable name="page" 
		    select="document ( $pagename )"/>
      <xsl:if test="string-length($page) = 0">
	<xsl:message>	WARNING! Page file '<xsl:value-of select="@file"/>' not found.</xsl:message>
	<xsl:message>	WARNING! (.xsl) Looked for page '<xsl:value-of select="$pagename"/>'</xsl:message>
      </xsl:if>
      <xsl:if test="not($page/esp:page)">
	<xsl:message>   WARNING! Page file '<xsl:value-of select="@file"/>' must begin with 'page' element in 'http://oracc.org/ns/esp/1.0' namespace.</xsl:message>
      </xsl:if>
      <xsl:apply-templates select="$page/esp:page/*"/>
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
  <xsl:template match="esp:* | eb:* | h:html | h:head | h:link | h:body | h:meta | h:p | h:pre
		       | h:ol | h:ul | h:li | h:em | h:b | h:i | h:q | h:blockquote 
		       | h:abbr | h:acronym | h:br | h:wbr
		       | h:h1 | h:h2 | h:h3 | h:h4 | h:h5 | h:h6 | h:code | h:kbd 
		       | h:table | h:th | h:tr | h:td | h:thead | h:tbody | h:caption | h:tfoot
		       | h:dl | h:dd | h:dt | h:strong | h:style | h:sup | h:sub | h:address 
		       | h:object | h:param | h:embed | h:div | h:script | h:small | h:iframe
		       | h:span | h:form | h:input | h:select | h:option | h:hr | h:audio | h:source">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <!-- strip unrecognised tags and warn -->
  <xsl:template match="*">
    <xsl:message>	WARNING! Unrecognised element '<xsl:value-of select="name ()"/>' was filtered.</xsl:message>
    <xsl:apply-templates/>
  </xsl:template>

</xsl:stylesheet>
