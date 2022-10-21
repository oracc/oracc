<?xml version='1.0' encoding="utf-8"?>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xtl="http://oracc.org/ns/list/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xmd xtl xtr">

<xsl:param name="project"/>
<xsl:output method="xml" omit-xml-declaration="yes" indent="yes"/>

<xsl:template match="xtl:list">
  <xsl:message>hello3</xsl:message>
  <div class="xtloutline border-top border-bottom">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="xtl:group">
  <div id="{@type}">
    <h3 class="xtl h3"><xsl:value-of select="@type"/></h3>
    <div class="xol2">
      <xsl:choose>
	<xsl:when test="count(xtl:item)>0">
	  <xsl:for-each select="xtl:item">
	    <xsl:sort select="string-length(@sig)"/>
	    <xsl:sort select="@sig"/>
	    <xsl:apply-templates select="."/>
	  </xsl:for-each>
	</xsl:when>
	<xsl:otherwise>
	  <p><xsl:text> </xsl:text></p>
	</xsl:otherwise>
      </xsl:choose>
    </div>
  </div>
</xsl:template>

<xsl:template match="xtl:item">
  <xsl:variable name="type">
    <xsl:choose>
      <xsl:when test="starts-with(@text,'P')">exemplar</xsl:when>
      <xsl:when test="starts-with(@text,'Q')">composite</xsl:when>
      <xsl:otherwise/>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="image-nodes" select="document(concat(@path,'/',@text,'.xmd'))/*/xmd:cat/xmd:images/*"/>
  <xsl:variable name="image" select="count($image-nodes)"/>
  <xsl:variable name="best-image">
    <xsl:choose>
      <xsl:when test="$image-nodes[@type='p']">
	<xsl:value-of select="$image-nodes[@type='p' and @scale='full']/@src"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$image-nodes[@type='l' and @scale='full']/@src"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="imgclass">
    <xsl:choose>
      <xsl:when test="$image > 0">withimg</xsl:when>
      <xsl:otherwise>sansimg</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <p class="xo {$imgclass}">
    <xsl:choose>
      <xsl:when test="$image > 0">
<!--	<a target="_blank" href="http://cdli.ucla.edu/dl/{$best-image}" class="haveimg">IMG</a> -->
<!-- Dear Steve: one day you will come back here to do per-project
     image access.  When you do, remember that pqx_map in the resolver
     fails when the project doesn't have the P-number in its own
     catalogue -->
        <a target="_blank"
	   href="http://oracc.museum.upenn.edu/cdli/{@text}/image"
	   class="haveimg">IMG</a>
      </xsl:when>
      <xsl:otherwise>
	<span class="noimg">&#xa0;</span>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="@sig">
      <xsl:value-of select="concat(' [', @sig, '] ')"/>
    </xsl:if>
    <xsl:choose>
      <xsl:when test="@proj = 'cdli'">
	<a href="javascript:showcdli('{@text}')">
	  <xsl:value-of select="@n"/>
	</a>
      </xsl:when>
      <xsl:when test="@have_atf = 'no'">
	<a href="javascript:showcdli('{@text}')">
	  <xsl:value-of select="@n"/>
	</a>
      </xsl:when>
      <xsl:otherwise>
	<a href="javascript:showexemplar('{@proj}','{@text}','','')">
	  <xsl:value-of select="@n"/>
	</a>
      </xsl:otherwise>
    </xsl:choose>
  </p>
</xsl:template>

</xsl:stylesheet>
