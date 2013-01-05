<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:c="http://oracc.org/ns/cbd/1.0"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:s="http://oracc.org/ns/sortkey/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:ex="http://exslt.org/common"
  extension-element-prefixes="ex"
  exclude-result-prefixes="c g s">

<xsl:key name="xcpd" match="c:cpd" use="@eref"/>
<xsl:key name="xsig" match="c:cpd" use="@esig"/>

<xsl:template match="c:entry/c:bases">
  <xsl:choose>
    <xsl:when test="c:base[@primary='1']">
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:apply-templates/>
      </xsl:copy>
    </xsl:when>
    <xsl:when test="c:base[@icount]">
      <!-- FIXME: sort reversed by @icount and tag first entry as primary ?-->
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:apply-templates/>
      </xsl:copy>
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:apply-templates mode="primary" select="c:base[1]"/>
	<xsl:apply-templates select="c:base[position()>1]"/>
      </xsl:copy>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="primary" match="c:base">
  <xsl:copy>
    <xsl:attribute name="primary">1</xsl:attribute>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>