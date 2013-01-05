<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:epad="http://psd.museum.upenn.edu/epad/"
  xmlns:ex="http://exslt.org/common"
  extension-element-prefixes="epad ex">

<xsl:import href="g2-gdl-HTML.xsl"/>
<xsl:include href="html-standard.xsl"/>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:param name="project" select="/*/@project"/>
<xsl:param name="webdir"/>

<xsl:variable name="jsBegin">javascript:cbdResultsFromOutline('</xsl:variable>
<xsl:variable name="jsEnd">')</xsl:variable>

<xsl:variable name="lower" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="upper" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:template match="cbd:articles">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="/*/@project"/>
    <xsl:with-param name="with-hr" select="false()"/>
    <xsl:with-param name="with-trailer" select="false()"/>
    <xsl:with-param name="title" select="TOC"/>
    <xsl:with-param name="webtype" select="'p2-p1'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="cbd:letter">
  <xsl:variable name="c">
    <xsl:choose>
      <xsl:when test="@dc:title='Ĝ'"><xsl:value-of select="'GN'"/></xsl:when>
      <xsl:when test="@dc:title='Š'"><xsl:value-of select="'SH'"/></xsl:when>
      <xsl:when test="@dc:title='Ś'"><xsl:value-of select="'SS'"/></xsl:when>
      <xsl:when test="@dc:title='Ŋ'"><xsl:value-of select="'GN'"/></xsl:when>
      <xsl:when test="@dc:title='Ṣ'"><xsl:value-of select="'ST'"/></xsl:when>
      <xsl:when test="@dc:title='Ṭ'"><xsl:value-of select="'TT'"/></xsl:when>
      <xsl:when test="@dc:title='Ş'"><xsl:value-of select="'ST'"/></xsl:when>
      <xsl:when test="@dc:title='Ţ'"><xsl:value-of select="'TT'"/></xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="@dc:title"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="summaries-file">
    <xsl:value-of select="concat('/cbd/',/*/@xml:lang,'/summaries-',$c,'.html')"/>
  </xsl:variable>
  <p class="toc-entry">
    <a href="{concat($jsBegin,'/',/*/@project,$summaries-file,$jsEnd)}">
      <xsl:value-of select="@dc:title"/>
    </a>
  </p>
  <ex:document href="{concat('./',$webdir,$summaries-file)}"
	       method="xml" encoding="utf-8"
	       doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
	       doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
	       indent="yes">
    <xsl:call-template name="make-html">
      <xsl:with-param name="project" select="/*/@project"/>
      <xsl:with-param name="with-hr" select="false()"/>
      <xsl:with-param name="with-trailer" select="false()"/>
      <xsl:with-param name="title" select="Summary"/>
    </xsl:call-template>
  </ex:document>
</xsl:template>

<xsl:template name="call-back">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="cbd:entry">
  <p class="summary" id="{@ref}">
    <xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
    <xsl:copy-of select="@*[not(self::xml:id)]|cbd:overview/@periods"/>
    <xsl:variable name="js">
      <xsl:text>'</xsl:text>
      <xsl:value-of select="@ref"/>
      <xsl:text>.html'</xsl:text>
    </xsl:variable>
    <!-- note: callers may want to get the following span and put a br before it -->
    <span class="summary">
      <span class="summary-headword">
	<xsl:variable name="href">
	  <xsl:text>/cgi-bin/oracc?prod=ecbd&amp;project=</xsl:text>
	  <xsl:value-of select="$project"/>
	  <xsl:text>&amp;item=</xsl:text>
	  <xsl:value-of select="@ref"/>
	  <xsl:text>&amp;lang=</xsl:text>
	  <xsl:value-of select="/*/@xml:lang"/>
	</xsl:variable>
	<a href="{$href}">
	  <xsl:apply-templates select="cbd:cf|cbd:gw|cbd:pos|cbd:root"/>
	</a>
	<xsl:if test="@icount">
	  <xsl:text> (</xsl:text>
	  <xsl:value-of select="@icount"/>
	  <xsl:text>x)</xsl:text>
	</xsl:if>
	<xsl:text> </xsl:text>
      </span>
      <xsl:value-of select="@periods"/><xsl:text> </xsl:text>

<!--      <xsl:apply-templates select=".//cbd:t|.//cbd:mng|.//cbd:term"/> -->
      <xsl:call-template name="list">
	<xsl:with-param name="nodes" select="./*/cbd:base/cbd:t[1]"/>
	<xsl:with-param name="before" select="' wr. '"/>
	<xsl:with-param name="between" select="'; '"/>
	<xsl:with-param name="after" select=". "/>
      </xsl:call-template>
    </span>
  </p>
</xsl:template>

<xsl:template match="cbd:cf">
  <span class="cf"><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="cbd:gw">
  <xsl:text> [</xsl:text>
  <span class="gw"><xsl:value-of select="translate(text(),$lower,$upper)"/></span>
  <xsl:text>]</xsl:text>
</xsl:template>

<xsl:template match="cbd:pos">
  <xsl:text> </xsl:text>
  <span class="cf"><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="cbd:root">
  <xsl:value-of select="concat(' (&#x221A;',text(),')')"/>
</xsl:template>

<xsl:template name="list">
  <xsl:param name="nodes"/>
  <xsl:param name="before"/>
  <xsl:param name="between"/>
  <xsl:param name="after"/>
  <xsl:value-of select="$before"/>
  <xsl:for-each select="$nodes">
    <xsl:apply-templates select="."/>
    <xsl:if test="not(position()=last())">
      <xsl:value-of select="$between"/>
    </xsl:if>
  </xsl:for-each>
  <xsl:value-of select="$after"/>
</xsl:template>

<xsl:template match="cbd:t">
  <span class="wr"><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="cbd:mng">
  <xsl:choose>
    <xsl:when test="not(preceding-sibling::cbd:mng)">
      <xsl:text> "</xsl:text>
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>; </xsl:text>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="not(following-sibling::cbd:mng)">
    <xsl:text>"</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="cbd:term">
  <xsl:choose>
    <xsl:when test="not(preceding-sibling::cbd:term)">
      <xsl:text> Akk.&#xa0;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>; </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <span class="akk"><xsl:apply-templates/></span>
  <xsl:if test="not(following-sibling::cbd:term)">
<!--    <xsl:text>.</xsl:text> -->
  </xsl:if>
</xsl:template>

<xsl:template match="cbd:sup">
  <sup><xsl:apply-templates/></sup>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
