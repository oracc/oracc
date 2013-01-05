<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns="http://www.w3.org/1999/xhtml"
    version="1.0">

<xsl:output method="xml" encoding="utf-8" omit-xml-declaration="yes"/>

<xsl:template match="/">
  <xsl:apply-templates select="*/project-stats"/>
</xsl:template>

<xsl:template match="project-stats">
  <html>
    <head>
      <title>
	<xsl:value-of select="document('config.xml', /)/*/@n"/>
	<xsl:text> project statistics</xsl:text>
      </title>
      <link rel="stylesheet" type="text/css" href="/css/stats.css"/>
    </head>
    <body>
<!--
      <h1>
	<xsl:value-of select="document('config.xml', /)/*/@n"/>
	<xsl:text> project statistics</xsl:text>
      </h1>
-->
      <table class="stats">
	<xsl:apply-templates select="*[not(.='0')]"/>
      </table>
    </body>
  </html>
</xsl:template>

<xsl:template match="individual_texts">
  <tr>
    <td class="type">Individual manuscripts in catalogue</td>
    <td class="count"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="composite_texts">
  <tr>
    <td class="type">Composite texts in catalogue</td>
    <td class="count"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="individual_atfs">
  <tr>
    <td class="type">Individual witnesses transliterated</td>
    <td class="count"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="composite_atfs">
  <tr>
    <td class="type">Composite texts transliterated</td>
    <td class="count"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="individual_lemm">
  <tr>
    <td class="type">Individual witnesses lemmatized</td>
    <td class="count"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="composite_lemm">
  <tr>
    <td class="type">Composite texts lemmatized</td>
    <td class="count"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="wordcount">
  <tr>
    <td class="type">Number of words in corpus</td>
    <td class="count"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="attested_signatures">
  <tr>
    <td class="type">Distinct lemmatizations</td>
    <td class="count"><xsl:apply-templates/></td>
  </tr>
</xsl:template>

<xsl:template match="lemmed_words">
  <tr>
    <td class="type">Lemmatized words</td>
    <td class="count"><xsl:apply-templates/></td>
  </tr>
</xsl:template>


</xsl:transform>