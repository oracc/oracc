<?xml version='1.0'?>

<!-- modify the HTML for the one-big-file version -->

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:ex="http://exslt.org/common"
  xmlns:i="http://oracc.org/ns/instances/1.0"
  xmlns:stt="http://oracc.org/ns/stats/1.0"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="ex"
  exclude-result-prefixes="cbd dc ex i xl stt">

<xsl:param name="dateversion"/>
<xsl:param name="project"/>

<xsl:output method="xml" encoding="utf-8" indent="yes"/>

<!--
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
  -->

<xsl:template match="xh:body">
  <body id="p4Pager" class="obf">
    <xsl:choose>
      <xsl:when test="string-length(@data-proj)>0">
	<xsl:copy-of select="@data-proj"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:attribute name="data-proj"><xsl:value-of select="$project"/></xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <div class="obf-header">
      <h1><xsl:value-of select="/*/xh:head/xh:title"/></h1>
      <h2>Version of <xsl:value-of select="$dateversion"/></h2>
    </div>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="xh:div[contains(@class,'letter')]">
  <xsl:if test="*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <h1 class="obf-letter">
	<span class="obf-letter"><xsl:value-of select="@name"/></span>
      </h1>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:if>
</xsl:template>

<xsl:template match="xh:h1[@class='entry']">
  <h1 class="obf-entry heading">
    <xsl:apply-templates/>
  </h1>
</xsl:template>

<xsl:template match="xh:div[@class='summary']"/>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
