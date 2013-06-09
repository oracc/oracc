<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:xh="http://www.w3.org/1999/xhtml"
		xmlns:xs="http://www.w3.org/2001/XMLSchema"
		xmlns:ev="http://www.w3.org/2001/xml-events"
		xmlns:xpd="http://oracc.org/ns/xpd/1.0"
		exclude-result-prefixes="xh"
		>

<xsl:include href="xpd.xsl"/>

<xsl:param name="project"/>

<xsl:variable name="oracc-home" select="'/usr/local/oracc/'"/>
<xsl:variable name="oracc-server" select="'http://oracc.museum.upenn.edu/'"/>
<xsl:variable name="project-data" 
	      select="concat('/usr/local/oracc/xml/',$project,'/project-data.xml')"/>
<xsl:variable name="config-xml" 
	      select="document(concat('/usr/local/oracc/www/',$project,'/config.xml'))"/>
<xsl:variable name="project-abbrev"
	      select="$config-xml/*/xpd:abbrev"/>

<xsl:variable name="nth-child" 
	      select="'.pll .value select option:nth-child('"/>
<xsl:variable name="border-top"
	      select="') { border-top: 1px solid red; }&#xa;'"/>

<xsl:output method="xml" encoding="utf-8" omit-xml-declaration="yes" indent="yes"/>

<xsl:template match="xh:html">
<!--  <xsl:processing-instruction name="xml-stylesheet"
			      >href="/xsltforms/xsltforms/xsltforms.xsl" type="text/xsl"</xsl:processing-instruction> -->
<!--  <xsl:processing-instruction name="xsltforms-options"
			      >debug="yes"</xsl:processing-instruction>
  <xsl:text>&#xa;</xsl:text> -->
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:head">
  <xsl:copy>
    <xsl:apply-templates/>
    <xsl:call-template name="pll-css"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:input[@name='project']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="value">
      <xsl:value-of select="$config-xml/*/@n"/>
    </xsl:attribute>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:p[@class='glossaries']|xh:span[contains(@class,'glossaries')]">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <select name="setglo" id="setglo" onchange="p3do('setglo');submit()">
      <option value="#none">Glossaries</option>
      <xsl:for-each select="document($project-data)//glossary">
	<option value="{@abbrev}"><xsl:value-of select="."/></option>
      </xsl:for-each>
    </select>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:p[@class='outline-sorter']|xh:span[@class='outline-sorter']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="document($project-data)//outline-sorters/*"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:span[@id='xpd-abbrev']">
  <xsl:value-of select="$config-xml/*/xpd:abbrev"/>
</xsl:template>

<xsl:template match="xh:span[@id='xpd-name']">
  <xsl:value-of select="$config-xml/*/xpd:name"/>
</xsl:template>

<xsl:template match="xh:span[@class='translations']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <select name="setlang" id="setlang" onchange="p3do('setlang');submit()">
      <xsl:for-each select="document($project-data)//lang">
	<option value="{@abbrev}"><xsl:value-of select="."/></option>
      </xsl:for-each>
    </select>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:span[@class='pll']|xh:span[contains(@class,'pll')]">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <select name="seturl" id="seturl" onchange="p3do('seturl');submit()">
      <option value="oracc">Oracc Home</option>
      <option value="project">Project Home</option>
      <xsl:for-each select="document($project-data)/*/projects/project">
	<option value="/{@proj}"><xsl:value-of select="@menu"/></option>
      </xsl:for-each>
      <xsl:for-each select="document($project-data)/*/lists/list">
	<option value="/{@proj}/{@file}"><xsl:value-of select="@menu"/></option>
      </xsl:for-each>
      <xsl:for-each select="document($project-data)/*/links/link">
	<option value="/{@url}"><xsl:value-of select="@menu"/></option>
      </xsl:for-each>
    </select>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template name="pll-css">
  <style type="text/css">
    <xsl:text>.pll .value select option { border-bottom: 1px solid red; }&#xa;</xsl:text>
<!--
    <xsl:text>.pll .value select option:nth-child(3) { border-top: 1px solid red; }&#xa;</xsl:text>
    <xsl:call-template name="pll-lists-nth"/>
    <xsl:call-template name="pll-links-nth"/>
 -->
</style><xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="pll-lists-nth">
  <xsl:if test="count(document($project-data)/*/projects/project) > 0">
    <xsl:variable name="nth" select="2 + count(document($project-data)/*/projects/project)"/>
    <xsl:value-of select="concat($nth-child,$nth, $border-top)"/>
  </xsl:if>
</xsl:template>

<xsl:template name="pll-links-nth">
  <xsl:if test="count(document($project-data)/*/projects/project) > 0
		+ count(document($project-data)/*/lists/list) > 0">
    <xsl:variable name="nth" select="2 + count(document($project-data)/*/projects/project)
				     + count(document($project-data)/*/lists/list)"/>
    <xsl:value-of select="concat($nth-child,$nth, $border-top)"/>
  </xsl:if>
</xsl:template>

<!--
.pll .value select option:nth-child(1),
.pll .value select option:nth-child(2),
.pll .value select option:nth-child(3)
 { border-bottom: 1px solid red; }
 -->
</xsl:stylesheet>
