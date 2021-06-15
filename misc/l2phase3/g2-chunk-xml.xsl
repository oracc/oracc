<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:ex="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="ex"
  exclude-result-prefixes="xh xl dc cbd">

<xsl:param name="subdir"/> <!-- output goes here -->
<xsl:param name="name"/>   <!-- file is called name-01.xml etc. -->

<xsl:variable name="subdir-str">
  <xsl:choose>
    <xsl:when test="string-length($subdir) > 0">
      <xsl:value-of select="concat($subdir,'/')"/>
    </xsl:when>
    <xsl:otherwise/>
  </xsl:choose>
</xsl:variable>

<xsl:output method="xml" encoding="utf-8" indent="no"/>

<xsl:template match="cbd:letter">
  <xsl:if test="*">
    <xsl:variable name="nth" select="1+count(preceding-sibling::*)"/>
    <xsl:variable name="chunk">
      <xsl:choose>
	<xsl:when test="$nth &lt; 10">
	  <xsl:value-of select="concat('0',$nth)"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="$nth"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <ex:document href="{concat($subdir,'/',$name,'-',$chunk,'.xml')}"
		 method="xml" encoding="utf-8"
		 indent="no">
      <cbd:articles>
	<xsl:copy-of select="ancestor::cbd:articles/@*"/>
	<xsl:attribute name="chunk"><xsl:value-of select="$chunk"/></xsl:attribute>
	<xsl:copy-of select="*"/>
      </cbd:articles>
    </ex:document>
  </xsl:if>
</xsl:template>
  
</xsl:stylesheet>
