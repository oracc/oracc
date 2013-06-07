<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:ce="http://oracc.org/ns/ce/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="ce dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:import href="html-standard.xsl"/>
<xsl:import href="gdl-HTML.xsl"/>

<xsl:param name="hits"/>
<xsl:param name="query"/>
<xsl:param name="page-of-page"/>
<xsl:param name="next-page"/>
<xsl:param name="prev-page"/>

<xsl:template match="ce:ce">
  <xsl:call-template name="make-html">
    <xsl:with-param name="title" select="'ePSD2'"/>
    <xsl:with-param name="project" select="epsd2'"/>
    <xsl:with-param name="webtype" select="'cbd'"/>
    <xsl:with-param name="with-hr" select="false()"/>
    <xsl:with-param name="with-trailer" select="false()"/>
  </xsl:call-template>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template name="call-back">
  <div class="tocbanner">
    <p class="toctitle">ePSD2</p>
    <p class="tocletter">$hits for $query</p>
    <p class="toccenter">
      <xsl:if test="string-length($prev-page) > 0">
	<a href="/cgi-bin/epsdse?p={$prev-page}&amp;k1={$query}">
	  <span class="arrow">&#xAB;</span>
	</a>
      </xsl:if>
      <xsl:value-of select="$page-of-page/>
      <xsl:if test="string-length($next-page) > 0">
	<a href="/cgi-bin/epsdse?p={$next-page}&amp;k1={$query}">
	  <span class="arrow">&#xBB;</span>
	</a>
      </xsl:if>
    </p>
  </div>
  <xsl:for-each select="ce:data/*">
    <xsl:apply-templates mode="htmlify" select="."/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="*">
  <xsl:element name="{local-name(.)}">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="htmlify"/>
  </xsl:element>
</xsl:template>

</xsl:transform>
