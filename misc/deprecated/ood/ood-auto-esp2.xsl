<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
	       xmlns:gdf="http://oracc.org/ns/gdf/1.0"
	       xmlns="http://www.w3.org/1999/xhtml"
	       xmlns:xh="http://www.w3.org/1999/xhtml"
	       version="1.0">

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="gdf:metadata">
  <html>
    <head>
      <title>
	<xsl:value-of select="gdf:title"/>
      </title>
      <link rel="shortcut icon" type="image/ico" href="/favicon.ico"/>
      <link rel="stylesheet" type="text/css" href="/css/gdf.css"/>
      <xsl:if test="gdf:resources/gdf:have-css/text()='yes'">
	<link rel="stylesheet" type="text/css" href="{@gdf:abbrev}.css"/>
      </xsl:if>
      <xsl:if test="gdf:resources/gdf:have-js/text()='yes'">
	<script src="{@gdf:abbrev}.js" type="text/javascript">
	  <xsl:text> </xsl:text>
	</script>
      </xsl:if>
    </head>
    <body class="gdf">
      <div class="preamble">
	<p class="about">(This is an Oracc Generic Data Framework (GDF) dataset: <a href="/doc/ood/gdf/">learn more</a>)</p>
	<p class="description"><span class="meta-field">DATASET NAME: </span><xsl:value-of select="gdf:title"/></p>
	<p class="description"><span class="meta-field">DESCRIPTION: </span><xsl:value-of select="gdf:one-liner"/></p>
	<div class="metadata">
	  <p><span class="meta-field">STATUS: </span>
	  <xsl:for-each select="gdf:status/*">
	    <xsl:value-of select="local-name(.)"/><xsl:text>=</xsl:text><xsl:value-of select="."/>
	    <xsl:if test="not(position()=last())">
	      <xsl:text>; </xsl:text>
	    </xsl:if>
	  </xsl:for-each>
	  </p>
	</div>
<!--
	<xsl:if test="not(gdf:resources/gdf:have-xml/text()='no')">
	  <p>You can download the <a href="{/*/gdf:abbrev}.xml">XML version of the data</a> and 
	  use it under the terms of the CC BY-SA license.</p>
	</xsl:if>
	<xsl:if test="count(gdf:resources/gdf:original[@gdf:public='yes']) > 0">
	  <p>You can download the original data as:
	  <xsl:for-each select="gdf:resources/gdf:original[@gdf:public='yes']">
	    <a href="{.}"><xsl:value-of select="@gdf:format"/></a>
	    <xsl:if test="not(position()=last())">
	      <xsl:text>, </xsl:text>
	    </xsl:if>
	  </xsl:for-each>
	  <xsl:text>.</xsl:text></p>
	</xsl:if>
-->
      </div>
      <div class="search">
	<xsl:if test="not(gdf:resources/gdf:have-xml/text()='no')">
	  <p>You can <a href="/{gdf:owner-project}/data/{gdf:abbrev}/find">search this dataset using Oracc's Full Text Search</a>.</p>
	</xsl:if>
	<xsl:if test="string-length(gdf:id-first) > 0">
	  <p>You can <a href="/{gdf:owner-project}/data/{gdf:abbrev}/{gdf:id-first}">browse this dataset starting at the first record</a>.</p>
	</xsl:if>
      </div>
      <xsl:if test="gdf:index-page">
	<div class="index-page">
	  <xsl:apply-templates select="gdf:index-page/*"/>
	</div>
      </xsl:if>
    </body>
  </html>
</xsl:template>

<xsl:template match="xh:*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>