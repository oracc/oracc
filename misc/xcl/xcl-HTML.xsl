<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:ex="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  extension-element-prefixes="ex"
  exclude-result-prefixes="xcl xff xtf g ex">

<xsl:import href="xtf-HTML-lib.xsl"/>

<xsl:param name="unit-id"/>
<xsl:param name="proofing-mode"/>

<xsl:output method="xml" indent="no" encoding="utf-8"
	    omit-xml-declaration="yes"/>

<xsl:include href="gdl-chunk-renderer.xsl"/>

<xsl:variable name="xtfdoc" select="/"/>
<xsl:variable name="docroot" select="/"/>

<xsl:template match="/">
  <xsl:choose>
    <xsl:when test="xtf:xcl">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates select="/*/xcl:xcl"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xcl:xcl">
  <xsl:choose>
    <xsl:when test="string-length($unit-id) > 0">
      <xsl:apply-templates select="id($unit-id)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="newtree">
	<xtf:xcl>
	  <xsl:copy-of select="/*/@*"/>
	  <xsl:apply-templates mode="rewrite"/>
	</xtf:xcl>
      </xsl:variable>
      <xsl:variable name="ns" select="ex:node-set($newtree)"/>
      <!--  <xsl:copy-of select="$ns"/> -->
      <xsl:for-each select="$ns">
	<xsl:apply-templates select="."/>
      </xsl:for-each>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xcl:c[@type='sentence']">
  <p xcl:se_label="{/*/@n}, {@label}">
    <xsl:for-each select="*">
      <!-- <xsl:message>processing <xsl:value-of select="name(.)"/></xsl:message> -->
      <xsl:apply-templates mode="xcl-u-proof" select="."/>
    </xsl:for-each>
  </p>
</xsl:template>

<xsl:template mode="xcl-u-proof" match="xcl:c">
  <xsl:apply-templates mode="xcl-u-proof"/>
</xsl:template>

<xsl:template mode="xcl-u-proof" match="xcl:d">
  <xsl:if test="following-sibling::*">
    <xsl:choose>
      <xsl:when test="@type='line-start'">
	<span class="proof-lnum">
	  <xsl:value-of select="id(@ref)/@label"/>
	</span>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates select="id(@ref)"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template mode="xcl-u-proof" match="xcl:l">
  <xsl:variable name="lemform" select="xff:f"/>
  <span class="proof-table" id="{@xml:id}">
    <span class="proof-tr">
      <span class="proof-td">
	<xsl:for-each select="id(@ref)">
	  <xsl:call-template name="render-word"/>
	</xsl:for-each>
      </span>
    </span>
    <span class="proof-tr">
      <span class="proof-td">
	<img src="strut.png" width="0px" height="20pt" alt=""/>
	<xsl:value-of select="$lemform/@sublem"/>
	<span class="sub-strut"><img src="strut.png" width="0px" height="5pt" alt=""/></span>
      </span>
    </span>
    <xsl:choose>
      <xsl:when test="$lemform/@morph2">
	<span class="proof-tr">
	  <span class="proof-tr"><xsl:value-of select="$lemform/@morph"/></span>
	</span>
	<span class="proof-tr">
	  <span class="proof-td"><xsl:value-of select="$lemform/@morph2"/></span>
	</span>
      </xsl:when>
      <xsl:otherwise>
	<span class="proof-tr">
	  <span class="proof-td"><xsl:value-of select="$lemform/@norm0"/></span>
	</span>
      </xsl:otherwise>
    </xsl:choose>
  </span>
  <span><xsl:text> </xsl:text></span>
</xsl:template>

<xsl:template mode="rewrite" match="xcl:c[@type='sentence']">
  <xtf:u>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="rewrite"/>
  </xtf:u>
</xsl:template>

<xsl:template mode="rewrite" match="xcl:d[@type='line-start']">
  <xtf:lnum>
    <xsl:copy-of select="@*"/>
  </xtf:lnum>
</xsl:template>

<xsl:template mode="rewrite" match="xcl:l">
  <xsl:variable name="ref" select="@ref"/>
  <xsl:for-each select="$xtfdoc">
    <xsl:apply-templates mode="copy" select="id($ref)"/>
  </xsl:for-each>
</xsl:template>

<xsl:template mode="copy" match="*|text()">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="copy"/>
  </xsl:copy>
</xsl:template>

<xsl:template mode="rewrite" match="xcl:ll">
  <xsl:apply-templates mode="rewrite" select="xcl:l[1]"/>
</xsl:template>

<xsl:template mode="rewrite" match="xcl:c">
  <xsl:apply-templates mode="rewrite"/>
</xsl:template>

<xsl:template mode="rewrite" match="xcl:mds|xcl:linkbase"/>

<xsl:template mode="rewrite" match="*">
  <xsl:message>rewrite: tag '<xsl:value-of select="local-name()"/>' not handled</xsl:message>
</xsl:template>

<xsl:template match="xtf:xcl">
  <div>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="xtf:u">
  <p ref="{@xml:id}">
    <xsl:apply-templates>
      <xsl:with-param name="docroot" select="$docroot"/>
      <xsl:with-param name="proofing-mode" select="$proofing-mode"/>
    </xsl:apply-templates>
  </p>
</xsl:template>

<xsl:template match="xtf:lnum">
  <xsl:if test="preceding-sibling::*[1]"><br/></xsl:if>
  <sup><xsl:value-of select="@label"/></sup>&#xa0;
</xsl:template>

</xsl:stylesheet>
