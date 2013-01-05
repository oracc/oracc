<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:xh="http://www.w3.org/1999/xhtml"
		>

<xsl:include href="html-standard.xsl"/>
<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>
<xsl:param name="project" select="''"/>

<xsl:variable name="jsBegin">javascript:cbdResultsFromOutline('</xsl:variable>
<xsl:variable name="jsEnd">')</xsl:variable>

<xsl:template match="xh:div">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="title" select="'TOC'"/>
    <xsl:with-param name="webtype" select="'p2-p1'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <xsl:variable name="lang">
    <xsl:value-of select="substring-before(
			  substring-after(xh:p/xh:a/@href,'cbd/'),'/')"/>
  </xsl:variable>
<!--  <xsl:message>p2-cbd-toc: lang=<xsl:value-of select="$lang"/></xsl:message> -->
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <p class="toc-entry">
      <a>
	<xsl:attribute name="href">
	  <xsl:value-of select="concat($jsBegin,'/',$project,'/cbd/',$lang,'/p2-summaries.html', $jsEnd)"/>
	</xsl:attribute>
	Summaries
      </a>
    </p>
    <hr/>
    <xsl:apply-templates/>
    <hr/>
    <p class="toc-entry">
      <a>
	<xsl:attribute name="href">
	  <xsl:value-of select="concat($jsBegin,'/',$project,'/cbd/',$lang,'/onebigfile.html', $jsEnd)"/>
	</xsl:attribute>
	One Big File
      </a>
    </p>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:p">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:a">
  <xsl:copy>
    <xsl:copy-of select="@class"/>
    <xsl:attribute name="href">
      <xsl:value-of select="concat($jsBegin, @href, $jsEnd)"/>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
