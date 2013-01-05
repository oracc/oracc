<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
		xmlns:tei="http://www.tei-c.org/ns/1.0"
		version="1.0"
		exclude-result-prefixes="tei">

<xsl:include href="map-name.xsl"/>
<xsl:include href="map-name-fuzzy.xsl"/>
<xsl:include href="name-key.xsl"/>

<!-- if this is 1 then names which are already known are also output -->
<xsl:param name="harvest-all" select="0"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:key name="all-names" match="tei:author|tei:editor" use="text()"/>

<xsl:template match="/">
  <harvested-names>
    <xsl:apply-templates/>
  </harvested-names>
</xsl:template>

<xsl:template match="tei:author|tei:editor">
  <xsl:if test="generate-id(.)=generate-id(key('all-names',text()))">
    <xsl:variable name="try1">
      <xsl:call-template name="map-name"/>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$try1 and not($try1=text())">
	<xsl:if test="$harvest-all=1">
	  <name key="{$try1}" type="exact">
	    <aka><xsl:value-of select="text()"/></aka>
	  </name>
	</xsl:if>
      </xsl:when>
      <xsl:otherwise>
	<xsl:variable name="try2">
	  <xsl:call-template name="map-name-fuzzy"/>
	</xsl:variable>
	<xsl:choose>
	  <xsl:when test="string-length($try2)>0">
	    <name key="{$try2}" type="fuzzy">
	      <aka><xsl:value-of select="text()"/></aka>
	    </name>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:variable name="newkey">
	      <xsl:call-template name="name-key"/>
	    </xsl:variable>
	    <name key="{$newkey}" type="none">
	      <last><xsl:value-of select="substring-before(text(),', ')"/></last>
	      <rest><xsl:value-of select="substring-after(text(),', ')"/></rest>
	      <aka><xsl:value-of select="text()"/></aka>
	    </name>   
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>