<?xml version='1.0'?>

<xsl:stylesheet version="1.0"
  xmlns:ex="http://exslt.org/common"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:epad="http://psd.museum.upenn.edu/epad/"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="ex">

<xsl:import href="g2plus-index-driver.xsl"/>
<xsl:output method="xml" indent="no" encoding="utf-8"/>
<xsl:key name="id" match="cbd:entry" use="@xml:id"/>
<xsl:variable name="doc" select="/*"/>

<xsl:key name="common" match="common" use="@word"/>

<xsl:variable name="nonchars">
  <xsl:text>'"()?[]=</xsl:text>
</xsl:variable>

<xsl:variable name="nonchars-spaces">
  <xsl:text>        </xsl:text>
</xsl:variable>

<xsl:template match="/">
  <xsl:variable name="nodes-frag">
    <xsl:for-each select="//cbd:gw|//cbd:mng[text()]|//cbd:mean[text()]">
      <xsl:call-template name="split-outer">
	<xsl:with-param name="argstr" select="translate(text(),';',',')"/>
	<xsl:with-param name="id" select="ancestor::cbd:entry/@xml:id"/>
      </xsl:call-template>
    </xsl:for-each>
  </xsl:variable>
  <xsl:call-template name="make-index">
    <xsl:with-param name="basename" select="'english'"/>
    <xsl:with-param name="title" select="'English Index'"/>
    <xsl:with-param name="node-list" select="ex:node-set($nodes-frag)/*"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="make-sortkey">
  <xsl:call-template name="strip-leading-verbiage">
    <xsl:with-param name="str" select="."/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="make-what">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template name="make-where">
  <xsl:variable name="id" select="@entry"/>
  <xsl:for-each select="$doc">
    <xsl:value-of select="concat(key('id',$id)/cbd:cf,'[',key('id',$id)/cbd:gw,']')"/>
  </xsl:for-each>
</xsl:template>

<xsl:template name="make-href">
  <xsl:value-of select="concat(@entry,'.html')"/>
</xsl:template>

<xsl:template name="split-outer">
  <xsl:param name="argstr"/>
  <xsl:param name="id"/>
  <xsl:variable name="str" select="translate($argstr,$nonchars,$nonchars-spaces)"/>
<!--  <xsl:message>split-outer str = <xsl:value-of select="$str"/></xsl:message>-->
  <xsl:choose>
    <xsl:when test="not(contains($str,', '))">
      <xsl:call-template name="index-str">
  	<xsl:with-param name="str" select="$str"/>
	<xsl:with-param name="id" select="$id"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="index-str">
  	<xsl:with-param name="str" select="substring-before($str,', ')"/>
	<xsl:with-param name="id" select="$id"/>
      </xsl:call-template>
      <xsl:call-template name="split-outer">
	<xsl:with-param name="argstr" select="substring-after($str,', ')"/>
	<xsl:with-param name="id" select="$id"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="split-inner">
  <xsl:param name="str"/>
  <xsl:param name="id"/>
<!--  <xsl:message>split-inner str = <xsl:value-of select="$str"/></xsl:message>-->
  <xsl:choose>
    <xsl:when test="not(contains($str,','))">
      <xsl:call-template name="index-str">
  	<xsl:with-param name="str" select="$str"/>
	<xsl:with-param name="id" select="$id"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="index-str">
  	<xsl:with-param name="str" select="substring-before($str,',')"/>
	<xsl:with-param name="id" select="$id"/>
      </xsl:call-template>
      <xsl:call-template name="split-inner">
	<xsl:with-param name="str" select="substring-after($str,',')"/>
	<xsl:with-param name="id" select="$id"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="index-str">
  <xsl:param name="str"/>
  <xsl:param name="id"/>
  <xsl:variable name="istr">
    <xsl:call-template name="strip-leading-verbiage">
      <xsl:with-param name="str" select="$str"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:if test="string-length($istr) > 0">
    <xsl:variable name="common">
      <xsl:for-each select="document('../data/xix-common-en.xml')">
        <xsl:value-of select="count(key('common',$istr))"/>
      </xsl:for-each>
    </xsl:variable>
    <xsl:if test="$common = 0">
      <node entry="{$id}">
        <xsl:value-of select="$istr"/>
      </node>
    </xsl:if>
    <xsl:if test="contains($istr,' ')">
      <xsl:call-template name="split-inner">
        <xsl:with-param name="str" select="translate($istr, ' ', ',')"/>
        <xsl:with-param name="id" select="$id"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template name="strip-leading-verbiage">
  <xsl:param name="str"/>
  <xsl:choose>
    <xsl:when test="$str = 'to be'">
      <xsl:value-of select="$str"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'a class of ')">
      <xsl:value-of select="substring-after($str, 'a class of ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'a description of ')">
      <xsl:value-of select="substring-after($str, 'a description of ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'a kind of ')">
      <xsl:value-of select="substring-after($str, 'a kind of ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'a type of ')">
      <xsl:value-of select="substring-after($str, 'a type of ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'type of ')">
      <xsl:value-of select="substring-after($str, 'type of ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'a ')">
      <xsl:value-of select="substring-after($str, 'a ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'an ')">
      <xsl:value-of select="substring-after($str, 'an ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'the ')">
      <xsl:value-of select="substring-after($str, 'the ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'to be ')">
      <xsl:value-of select="substring-after($str, 'to be ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'to become ')">
      <xsl:value-of select="substring-after($str, 'to become ')"/>
    </xsl:when>
    <xsl:when test="starts-with($str, 'to ')">
      <xsl:value-of select="substring-after($str, 'to ')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$str"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
