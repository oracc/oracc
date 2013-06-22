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
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:head">
  <xsl:copy>
    <xsl:apply-templates/>
<!--    <xsl:call-template name="pll-css"/> -->
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:p[@class='glossaries']|xh:span[contains(@class,'glossaries')]">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <select id="setglo" onchange="selectGlossary('{$project}',this);">
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

<xsl:template match="xh:span[contains(@class,'back-to')]">
  <xsl:variable name="back-url" select="$config-xml/*/xpd:pager-back-url"/>
  <xsl:if test="string-length($back-url) > 0">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <button name="back-to" onclick="window.location='{$back-url}'">
	<xsl:choose>
	  <xsl:when test="string-length($config-xml/*/xpd:pager-back-url)">
	    <xsl:value-of select="$config-xml/*/xpd:pager-back-url"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>BACK</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
      </button>
    </xsl:copy>
  </xsl:if>
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
  <link rel="stylesheet" type="text/css" href="/{$project}/p2.css" />
  <style type="text/css">
    <xsl:text>.pll .value select option { border-bottom: 1px solid red; }&#xa;</xsl:text>
  </style>
  <xsl:text>&#xa;</xsl:text>
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

</xsl:stylesheet>
