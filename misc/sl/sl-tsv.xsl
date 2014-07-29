<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:template name="fields">
  <f>id</f>
  <f>sort</f>
  <f>sign</f>
  <f>form</f>
  <f>uhex</f>
  <f>uname</f>
  <f>lists</f>
  <f>value</f>
</xsl:template>

<xsl:template match="sl:signlist">
  <xsl:for-each select="document('')/*/xsl:template[@name='fields']/*">
    <xsl:value-of select="."/>
    <xsl:if test="not(position()=last())">
      <xsl:text>&#x9;</xsl:text>
    </xsl:if>
  </xsl:for-each>
  <xsl:text>&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="sl:form|sl:sign">
  <xsl:choose>
    <xsl:when test="sl:form">
      <xsl:for-each select="sl:v">
	<xsl:call-template name="row"/>
      </xsl:for-each>
      <xsl:for-each select="sl:form">
	<xsl:apply-templates select="."/>
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="sl:v">
      <xsl:for-each select="sl:v">
	<xsl:call-template name="row"/>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="row"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="row">
  <xsl:value-of select="ancestor-or-self::sl:sign/@xml:id"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="ancestor-or-self::sl:sign/sl:sort/@ogsl"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:choose>
    <xsl:when test="ancestor-or-self::sl:form">
      <xsl:value-of select="concat(ancestor::sl:sign/@n, '&#x9;', ancestor-or-self::sl:form/@n, '&#x9;')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="concat(ancestor-or-self::sl:sign/@n, '&#x9;', '', '&#x9;')"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:choose>
    <xsl:when test="sl:utf8">
      <xsl:value-of select="sl:utf8/@hex"/>
      <xsl:text>&#x9;</xsl:text>
      <xsl:value-of select="sl:uname"/>
      <xsl:text>&#x9;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="ancestor-or-self::sl:sign/sl:utf8/@hex"/>
      <xsl:text>&#x9;</xsl:text>
      <xsl:value-of select="ancestor-or-self::sl:sign/sl:uname"/>
      <xsl:text>&#x9;</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:for-each select="sl:list">
    <xsl:value-of select="@n"/>
    <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
  </xsl:for-each>
  <xsl:text>&#x9;</xsl:text>
  <xsl:if test="self::sl:v">
    <xsl:value-of select="@n"/>
    <xsl:if test="@uncertain='yes'"><xsl:text>?</xsl:text></xsl:if>
  </xsl:if>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

</xsl:transform>
