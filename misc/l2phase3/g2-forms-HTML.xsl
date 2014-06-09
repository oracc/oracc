<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:i="http://oracc.org/ns/instances/1.0"
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:note="http://oracc.org/ns/note/1.0"
  xmlns:n="http://oracc.org/ns/norm/1.0"
  xmlns:norm="http://oracc.org/ns/norm/1.0"
  xmlns:usg="http://oracc.org/ns/usg/1.0"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:oracc="http://oracc.org/ns/oracc/1.0"
  exclude-result-prefixes="dc i xl xcl norm usg note">

<xsl:include href="g2-gdl-HTML.xsl"/>

<xsl:key name="norms" match="cbd:norm/cbd:forms/cbd:f" use="@ref"/>

<xsl:variable name="basename" select="/*/@project"/>
<xsl:variable name="project" select="/*/@project"/>

<xsl:template match="/">
  <div>
    <xsl:for-each select="*/@n|*/@project|*/@xml:lang">
      <xsl:attribute name="oracc:{local-name()}"><xsl:value-of select="."/></xsl:attribute>
    </xsl:for-each>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="cbd:entry">
  <div class="ns-glo-sec">
    <xsl:attribute name="xml:id"><xsl:value-of select="concat(@xml:id,'.f')"/></xsl:attribute>
    <h1>Written forms attested for <xsl:value-of select="@n"/></h1>
    <xsl:for-each select="cbd:forms/cbd:form">
      <h2>
	<a class="icountu" href="javascript:distprof2('{$basename}','{/*/@xml:lang}','{@xis}')">
	  <xsl:apply-templates select="cbd:t[1]/*"/>
	  <xsl:value-of select="concat(' (',@icount,'x/',@ipct,'%)')"/>
	</a>
      </h2>
      <p class="ns-glo-form-sub">transcribed as:</p>
      <xsl:for-each select="key('norms',@xml:id)">
	  <xsl:for-each select="ancestor::cbd:norm">
	    <h3>
	      <a class="icountu" href="javascript:distprof2('{$basename}','{/*/@xml:lang}','{@xis}')">
		<xsl:value-of select="cbd:n"/>
		<xsl:value-of select="concat(' (',@icount,'x/',@ipct,'%)')"/>
	      </a>
	    </h3>
	  </xsl:for-each>
      </xsl:for-each>
    </xsl:for-each>
  </div>
</xsl:template>

<xsl:template match="cbd:articles|cbd:letter">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
