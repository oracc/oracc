<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:gdl="http://oracc.org/ns/gdl/1.0"
    xmlns:nrm="http://oracc.org/ns/nrm/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns:xmd="http://oracc.org/ns/xmd/1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    xmlns:xtr="http://oracc.org/ns/xtr/1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:key name="genres" match="xmd:genre" use="text()"/>
<xsl:key name="periods" match="xmd:period" use="text()"/>
<xsl:key name="places" match="xmd:provenience" use="text()"/>

<xsl:template match="/">
  <xsl:value-of select="concat('@uri=/',*/xpd:project/@n,'&#xa;')"/>
  <xsl:value-of select="concat('@title=', */xpd:project/xpd:abbrev, ': ', */xpd:project/xpd:name)"/>
  <xsl:text>&#xa;@weight=100000&#xa;</xsl:text>
  <xsl:text>&#xa;</xsl:text>
  <xsl:value-of select="*/xpd:project/xpd:blurb"/>
  <xsl:text>&#xa;&#x9;</xsl:text>
  <xsl:value-of select="*/xpd:project/xpd:blurb"/>
  <xsl:text>&#xa;&#x9;</xsl:text>
  <xsl:value-of select="*/xpd:project/xpd:blurb"/>
  <xsl:text>&#xa;&#x9;</xsl:text>  
  <xsl:for-each select="*/xmd:xmd/xmd:cat/xmd:id_text">
    <xsl:value-of select="."/>
    <xsl:text> </xsl:text>
  </xsl:for-each>
  <xsl:for-each select="*/xmd:xmd/xmd:cat/xmd:id_composite">
    <xsl:value-of select="."/>
    <xsl:text> </xsl:text>
  </xsl:for-each>
  <xsl:for-each select="*/xmd:xmd/xmd:cat">
    <xsl:for-each select="xmd:primary_publication|xmd:designation|xmd:collection
      |xmd:date_of_origin|xmd:dates_referenced|xmd:language|xmd:museum_no
      |xmd:publication_history|xmd:subgenre|xmd:text_remarks">
      <xsl:if test="string-length(text()) > 0">
	<xsl:text>&#x9;</xsl:text>
	<xsl:value-of select="text()"/>
	<xsl:text>&#xa;</xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:for-each>
  <xsl:text>&#xa;</xsl:text>
  <xsl:call-template name="xmd-values">
    <xsl:with-param name="keys" select="'genres'"/>
    <xsl:with-param name="nodes" select="*/xmd:xmd/xmd:cat/xmd:genre"/>
  </xsl:call-template>
  <xsl:text>&#xa;</xsl:text>
  <xsl:call-template name="xmd-values">
    <xsl:with-param name="keys" select="'periods'"/>
    <xsl:with-param name="nodes" select="*/xmd:xmd/xmd:cat/xmd:period"/>
  </xsl:call-template>
  <xsl:text>&#xa;</xsl:text>
  <xsl:call-template name="xmd-values">
    <xsl:with-param name="keys" select="'places'"/>
    <xsl:with-param name="nodes" select="*/xmd:xmd/xmd:cat/xmd:provenience"/>
  </xsl:call-template>
  <xsl:text>&#xa;</xsl:text>
  <xsl:text>&#x9; project </xsl:text>
  <xsl:value-of select="*/xpd:project/@n"/>
  <xsl:text> </xsl:text>
  <xsl:value-of select="*/xpd:project/xpd:abbrev"/>
  <xsl:text> </xsl:text>
  <xsl:value-of select="*/xpd:project/xpd:name"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="xmd-values">
  <xsl:param name="keys"/>
  <xsl:param name="nodes"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:for-each select="$nodes">
    <xsl:variable name="v" select="text()"/>
    <xsl:variable name="k" select="key($keys,$v)"/>
<!--
    <xsl:message><xsl:value-of select="count($k)"
    /> keys with value <xsl:value-of select="$v"
    /> in keys <xsl:value-of select="$keys"/></xsl:message>
    <xsl:message><xsl:value-of select="generate-id(.)"/>; <xsl:value-of select="generate-id($k[1])"/></xsl:message>
 -->
    <xsl:if test="generate-id(.)=generate-id($k[1])">
<!--      <xsl:message>printing</xsl:message> -->
      <xsl:for-each select="$k[position() &lt; 6]">
	<xsl:value-of select="$v"/>
	<xsl:text> </xsl:text>
      </xsl:for-each>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:transform>