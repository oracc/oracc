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
  exclude-result-prefixes="xh xl">

<xsl:template match="cbd:entries">
  <div class="toc-banner">
    <p>
      <xsl:for-each select="cbd:letter">
	<span class="toc-banner-entry" title="{@dc:title}">
	  <a href="/{/*/@project}/cbd/{/*/@xml:lang}/{@dc:title}.html"><xsl:value-of select="@dc:title"/></a>
	</span>
	<xsl:if test="not(position()=last())">
	  <span class="toc-banner-sep">|</span>
	</xsl:if>	
      </xsl:for-each>
    </p>
  </div>
</xsl:template>

<xsl:template match="*|text()"/>

</xsl:stylesheet>
