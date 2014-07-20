<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:note="http://oracc.org/ns/note/1.0"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  exclude-result-prefixes="note xmd xst xtr xh">

<xsl:output method="xml" encoding="utf-8" indent="yes" omit-xml-declaration="yes"/>

<xsl:include href="html-standard.xsl"/>

<xsl:template match="/">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="/*/@project"/>
    <xsl:with-param name="title" select="concat('Hub page for ', /*/@project)"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <div class="p3hub banner bborder-top bborder-bot">
    <h1>Hub page for <xsl:value-of select="/*/@project"/></h1>
  </div>
  <div class="p3hub">
    <h2>Core Resources</h2>

    <p><a href="/">Oracc Home Page</a></p>
    <p><a href="/doc2/help/">Oracc Documentation</a></p>
    <p><a href="/{/*/@project}/index.html"><xsl:value-of select="/*/@project"/> Home Page</a></p>
    <p><a href="/{/*/@project}/as"><xsl:value-of select="/*/@project"/> Advanced Search</a></p>

    <p><a href="/{/*/@project}/pager"><xsl:value-of select="/*/@project"/> Pager</a></p>

    <p><a href="/{/*/@project}/withatf">Transliterated Texts</a></p>
    <p><a href="/{/*/@project}/withlem">Lemmatized Texts</a></p>

    <xsl:if test="/*/glossaries/glogroup">
      <h2>Glossaries</h2>
      <table class="hubglo">
	<xsl:for-each select="/*/glossaries/glogroup">
	  <xsl:for-each select="*">
	    <xsl:variable name="first" select="document(concat('../01bld/',@abbrev,'/toc-banner.xml'),/)/*/*/*/*[1]/@title"/>
	    <tr>
	      <td><a href="/{/*/@project}/{@abbrev}"><xsl:value-of select="text()"/></a></td>
	      <td><a href="/{/*/@project}/cbd/{@abbrev}/summaries.html">Summaries</a></td>
	      <td><a href="/{/*/@project}/cbd/{@abbrev}/{$first}.html">Letter by Letter</a></td>
	      <td><a href="/{/*/@project}/cbd/{@abbrev}/onebigfile.html">One Big File</a></td>
	    </tr>
	  </xsl:for-each>
	</xsl:for-each>
      </table>
    </xsl:if>

    <xsl:if test="/*/projects/project[@type='sub']">
      <h2>Subprojects</h2>
      <xsl:for-each select="/*/projects/project[@type='sub']">
	<p><a href="/{@proj}/index.html"><xsl:value-of select="@menu"/></a></p>
      </xsl:for-each>
    </xsl:if>

    <xsl:if test="/*/lists/list">
      <h2>Special Lists</h2>
      <xsl:for-each select="/*/lists/*">
	<p><a href="/{/*/@project}/{@file}"><xsl:value-of select="@menu"/></a></p>
      </xsl:for-each>
    </xsl:if>

    <xsl:if test="/*/links/link">
      <h2>Additional Links</h2>
      <xsl:for-each select="/*/links/*">
	<p><a href="{@url}"><xsl:value-of select="@menu"/></a></p>
      </xsl:for-each>
    </xsl:if>
  </div>
</xsl:template>

</xsl:stylesheet>
