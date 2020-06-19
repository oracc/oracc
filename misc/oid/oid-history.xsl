<?xml version='1.0'?>

<xsl:stylesheet
    version="1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" encoding="utf-8" indent="yes" omit-xml-declaration="yes"/>

<xsl:param name="project"/>

<xsl:include href="html-standard.xsl"/>

<xsl:template match="/">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="title" select="'OID History'"/>
    <xsl:with-param name="body-class" select="'oid'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <h1><xsl:value-of select="$project"/> OID History</h1>
  <xsl:call-template name="about"/>
  <xsl:call-template name="oid-group">
    <xsl:with-param name="nodes" select="*/add"/>
    <xsl:with-param name="title" select="'Additions'"/>
  </xsl:call-template>
  <xsl:call-template name="oid-group">
    <xsl:with-param name="nodes" select="*/ren"/>
    <xsl:with-param name="title" select="'Renamings'"/>
  </xsl:call-template>
  <xsl:call-template name="oid-group">
    <xsl:with-param name="nodes" select="*/mrg"/>
    <xsl:with-param name="title" select="'Mergers'"/>
  </xsl:call-template>
  <xsl:call-template name="oid-group">
    <xsl:with-param name="nodes" select="*/del"/>
    <xsl:with-param name="title" select="'Deletions'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="oid-group">
  <xsl:param name="nodes"/>
  <xsl:param name="title"/>
  <h2><xsl:value-of select="$title"/></h2>
  <table>
    <colgroup>
      <col style="width: 10%"/>
      <col style="width: 10%"/>
      <col style="width: 30%"/>
      <col style="width: 30%"/>
      <col style="width: 20%"/>
    </colgroup>
    <xsl:apply-templates select="$nodes"/>
  </table>
</xsl:template>

<xsl:template match="add">
  <tr>
    <td><xsl:value-of select="@day"/></td>
    <td><a href="/{$project}/{@oid}"><xsl:value-of select="@oid"/></a></td>
    <td><xsl:value-of select="@cgp"/></td>
  </tr>
</xsl:template>

<xsl:template match="ren">
  <tr>
    <td><xsl:value-of select="@day"/></td>
    <td><a href="/{$project}/{@oid}"><xsl:value-of select="@oid"/></a></td>
    <td><xsl:value-of select="@cgp"/></td>
    <td><xsl:value-of select="@now-cgp"/></td>
  </tr>
</xsl:template>

<xsl:template match="mrg">
  <tr>
    <td><xsl:value-of select="@day"/></td>
    <td><a href="/{$project}/{@now-oid}"><xsl:value-of select="@now-oid"/></a></td>
    <td><xsl:value-of select="@cgp"/></td>
    <td><xsl:value-of select="@now-cgp"/></td>
  </tr>
</xsl:template>

<xsl:template match="del">
  <tr>
    <td><xsl:value-of select="@day"/></td>
    <td><xsl:value-of select="@oid"/></td>
    <td><xsl:value-of select="@cgp"/></td>
    <xsl:choose>
      <xsl:when test="string-length(@why-oid) > 0">
	<td>
	  <a href="/{$project}/{@why-oid}">
	    <xsl:value-of select="@why"/>
	  </a>
	</td>
      </xsl:when>
      <xsl:otherwise>
	<td><xsl:value-of select="@why"/></td>
      </xsl:otherwise>
    </xsl:choose>
  </tr>
</xsl:template>

<xsl:template name="about">
  
</xsl:template>

</xsl:stylesheet>
