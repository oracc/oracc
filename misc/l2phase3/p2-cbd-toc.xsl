<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:xh="http://www.w3.org/1999/xhtml"
		>

<xsl:include href="html-standard.xsl"/>
<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>
<xsl:param name="project" select="''"/>

<xsl:variable name="jsBegin">javascript:p3Letter('</xsl:variable>
<xsl:variable name="jsMid">','</xsl:variable>
<xsl:variable name="jsEnd">')</xsl:variable>

<xsl:variable name="lang">
  <xsl:value-of select="substring-before(
			substring-after(/*/xh:p[1]/xh:a/@href,'cbd/'),'/')"/>
</xsl:variable>
<xsl:variable name="base" select="concat('/',$project,'/cbd/',$lang,'/')"/>

<xsl:template match="xh:div">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="title" select="'TOC'"/>
    <xsl:with-param name="webtype" select="'p2-p1'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <p class="toc-entry">
      <a>
	<xsl:attribute name="href">
          <xsl:call-template name="p3Letter">
	    <xsl:with-param name="arg" select="'summaries'"/>
	  </xsl:call-template>
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
          <xsl:call-template name="p3Letter">
	    <xsl:with-param name="arg" select="'onebigfile'"/>
	  </xsl:call-template>
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
      <xsl:call-template name="p3Letter">
	<xsl:with-param name="arg" select="text()"/>
      </xsl:call-template>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template name="p3Letter">
  <xsl:param name="arg"/>
  <xsl:value-of select="concat($jsBegin,$arg,$jsEnd)"/>
</xsl:template>

</xsl:stylesheet>
