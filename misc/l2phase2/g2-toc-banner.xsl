<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:ex="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  extension-element-prefixes="ex"
  exclude-result-prefixes="xh xl dc cbd">

<xsl:template match="cbd:entries">
  <div class="toc-banner">
    <p>
      <xsl:for-each select="cbd:letter">
	<xsl:variable name="c">
	  <xsl:choose>
	    <xsl:when test="@dc:title='Ĝ'"><xsl:value-of select="'GN'"/></xsl:when>
	    <xsl:when test="@dc:title='Š'"><xsl:value-of select="'SH'"/></xsl:when>
	    <xsl:when test="@dc:title='Ś'"><xsl:value-of select="'SS'"/></xsl:when>
	    <xsl:when test="@dc:title='Ş'"><xsl:value-of select="'ST'"/></xsl:when>
	    <xsl:when test="@dc:title='Ţ'"><xsl:value-of select="'TT'"/></xsl:when>
	    <xsl:when test="@dc:title='Ŋ'"><xsl:value-of select="'GN'"/></xsl:when>
	    <xsl:when test="@dc:title='Ṣ'"><xsl:value-of select="'ST'"/></xsl:when>
	    <xsl:when test="@dc:title='Ṭ'"><xsl:value-of select="'TT'"/></xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="@dc:title"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<a href="/{/*/@project}/cbd/{/*/@xml:lang}/{$c}.html">
	  <span class="toc-banner-entry" title="{@dc:title}">
	    <xsl:value-of select="@dc:title"/>
	  </span>
	</a>
	<xsl:if test="not(position()=last())">
	  <span class="toc-banner-sep">|</span>
	</xsl:if>	
      </xsl:for-each>
    </p>
  </div>
</xsl:template>

<xsl:template match="*|text()"/>

</xsl:stylesheet>
