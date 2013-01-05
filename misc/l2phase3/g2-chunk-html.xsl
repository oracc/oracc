<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:ex="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="ex"
  exclude-result-prefixes="xh xl">

<xsl:import href="html-standard.xsl"/>

<xsl:param name="subdir"/>
<xsl:param name="webdir"/>

<xsl:variable name="subdir-str">
  <xsl:choose>
    <xsl:when test="string-length($subdir) > 0">
      <xsl:value-of select="concat($subdir,'/')"/>
    </xsl:when>
    <xsl:otherwise/>
  </xsl:choose>
</xsl:variable>

<xsl:output method="xml" encoding="utf-8" indent="no"/>

<xsl:param name="project"/>
<xsl:param name="title"/>
<xsl:param name="abbrev"/>
<xsl:param name="basename"/>
<xsl:param name="projectDir"/>

<xsl:template match="xh:head"/>

<xsl:template match="xh:body">
  <div class="toc">
    <xsl:attribute name="title">
      <xsl:value-of select="concat($abbrev,' Browsable HTML')"/>
    </xsl:attribute>
    <xsl:attribute name="basename">
      <xsl:value-of select="$basename"/>
    </xsl:attribute>
    <xsl:apply-templates select="xh:div[@class='letter']"/>
  </div>
</xsl:template>

<xsl:template match="xh:div[@class='letter']">
  <xsl:if test="*">
    <xsl:variable name="c">
      <xsl:choose>
	<xsl:when test="@id='Ĝ'"><xsl:value-of select="'GN'"/></xsl:when>
	<xsl:when test="@id='Š'"><xsl:value-of select="'SH'"/></xsl:when>
	<xsl:when test="@id='Ś'"><xsl:value-of select="'SS'"/></xsl:when>
	<xsl:when test="@id='Ş'"><xsl:value-of select="'ST'"/></xsl:when>
	<xsl:when test="@id='Ţ'"><xsl:value-of select="'TT'"/></xsl:when>
	<xsl:when test="@id='Ŋ'"><xsl:value-of select="'GN'"/></xsl:when>
	<xsl:when test="@id='Ṣ'"><xsl:value-of select="'ST'"/></xsl:when>
	<xsl:when test="@id='Ṭ'"><xsl:value-of select="'TT'"/></xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="@id"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="href" select="concat($c, '.html')"/>
    <xsl:variable name="toc-href" select="concat('/',$projectDir,'/cbd/',$subdir,'/', $href)"/>
    <xsl:choose>
      <xsl:when test="@id = 'Ĝ'">
	<p class="toc-entry"><a href="{$toc-href}"><xsl:value-of select="'Ŋ'"/></a></p>
      </xsl:when>
      <xsl:when test="@id = 'Ş'">
	<p class="toc-entry"><a href="{$toc-href}"><xsl:value-of select="'Ṣ'"/></a></p>
      </xsl:when>
      <xsl:when test="@id = 'Ţ'">
	<p class="toc-entry"><a href="{$toc-href}"><xsl:value-of select="'Ṭ'"/></a></p>
      </xsl:when>
      <xsl:otherwise>
	<p class="toc-entry"><a href="{$toc-href}"><xsl:value-of select="@id"/></a></p>
      </xsl:otherwise>
    </xsl:choose>
    <ex:document href="{concat('./',$webdir,'/cbd/',$subdir-str,$href)}"
		 method="xml" encoding="utf-8"
		 doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
		 doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
		 indent="yes">
      <xsl:call-template name="make-html">
	<xsl:with-param name="project" select="$project"/>
	<xsl:with-param name="webtype" select="'cbd'"/>
	<xsl:with-param name="with-trailer" select="false()"/>
      </xsl:call-template>
    </ex:document>
  </xsl:if>
</xsl:template>
  
<xsl:template name="call-back">
  <xsl:attribute name="class">obf</xsl:attribute>
  <div class="obf-header">
    <h1 class="obf-letter">
      <span class="obf-letter">
	<xsl:value-of select="$abbrev"/><xsl:text> Browsable HTML: </xsl:text>
	<xsl:value-of select="@id"/>
      </span>
    </h1>
    <h2 class="obf-letter">
      <xsl:apply-templates select="/*/xh:body/xh:div[@class='obf-header']/xh:h2"/>
    </h2>
  </div>
  <div class="letter">
    <xsl:copy-of select="*[position()>1]"/>
  </div>
</xsl:template>

</xsl:stylesheet>
