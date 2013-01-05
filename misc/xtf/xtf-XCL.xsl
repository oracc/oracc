<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:xtf1="http://oracc.org/ns/xtf/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:gdl="http://oracc.org/ns/gdl/1.0"
  xmlns:n="http://oracc.org/ns/norm/1.0"
  xmlns:syn="http://oracc.org/ns/syntax/1.0"
  xmlns="http://oracc.org/ns/xcl/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xtf gdl">

<!--  doctype-system="http://enlil.museum.upenn.edu/oracc/tools/ORACC/XCL/xcl.dtd" -->

<xsl:strip-space elements="*"/>
<xsl:output method="xml" 
  indent="no" encoding="utf-8"/>

<xsl:template match="xtf:xtf">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:transliteration|xtf:composite|xtf1:composite">
  <c type="top">
    <xsl:attribute name="ref"><xsl:value-of select="/*/@xml:id"/></xsl:attribute>
    <xsl:attribute name="n"><xsl:value-of select="/*/@n"/></xsl:attribute>
<!--    <xsl:if test="not(xtf:object) and (not(xtf:div) or xtf:l)"> -->
      <xsl:call-template name="c-text"/>
<!--    </xsl:if> -->
    <xsl:apply-templates/>
  </c>
</xsl:template>

<xsl:template match="xtf:sealing|xtf:surface[@type='seal']">
  <xsl:call-template name="c-text"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:object">
  <xsl:call-template name="c-text"/>
  <xsl:apply-templates/>
</xsl:template>

<!-- Divisions start new chunks ; really?  experimenting with
     discontinuities instead ...  -->
<xsl:template match="xtf:div|xtf1:div">
  <xsl:call-template name="div-start"/>
  <xsl:apply-templates/>
  <xsl:call-template name="div-end"/>
</xsl:template>

<xsl:template match="xtf:surface|xtf:column">
  <xsl:apply-templates/>
</xsl:template>

<!-- Milestones are separate chunks if they mark discourse units 
     or text divisions; locators are just discontinuities that
     can push headings into the stream -->
<xsl:template match="xtf:m|xtf1:m">
  <xsl:choose>
    <xsl:when test="@type='text'">
      <c type="discourse" subtype="{@subtype}"/>
    </xsl:when>
    <xsl:when test="@type='division'">
      <c type="block" subtype="{@subtype}" level="2"/>
    </xsl:when>
    <xsl:when test="@type='locator'">
      <d type="locator" subtype="{@subtype}" label="{text()}"/>
    </xsl:when>
    <xsl:when test="@type='discourse'">
      <d type="discourse" subtype="{@subtype}"/>
    </xsl:when>
    <xsl:otherwise>
      <d type="milestone" subtype="{text()}"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xtf:lg|xtf1:lg">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:l|xtf1:l">
  <xsl:choose>
    <xsl:when test="not(@type)">
      <d type="line-start">
	<xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
	<xsl:attribute name="label"><xsl:value-of select="@label"/></xsl:attribute>
      </d>
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise/>
  </xsl:choose>
</xsl:template>

<xsl:template match="xtf:nonx">
  <!-- only operate on reportable noncol/line -->
  <xsl:if test="contains(@type,'missing') or contains(@type,'traces') or contains(@type,'blank')">
    <d type="break" form="{@type}">
      <xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
    </d>
  </xsl:if>
</xsl:template>

<xsl:template match="xtf:nong|xtf1:nonl">
  <xsl:if test="not(ancestor::gdl:w) and (@type = 'broken' or @type = 'traces')">
    <w type="broken" form="X">
      <xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
    </w>
  </xsl:if>
</xsl:template>

<!--
<xsl:template match="xtf:n">
  <d type="num-start" sys="{@system}"/>
    <xsl:apply-templates/>
  <d type="num-end"/>
</xsl:template>
-->

<xsl:template match="xtf:c">
  <d type="cell-start" utype="{@type}"/>
    <xsl:apply-templates/>
  <d type="cell-end"/>
</xsl:template>

<xsl:template match="xtf:f">
  <c type="field-start" utype="{@type}">
    <xsl:attribute name="xml:lang">
      <xsl:value-of select="ancestor-or-self::*[@xml:lang][1]/@xml:lang"/>
    </xsl:attribute>
    <xsl:apply-templates/>
  </c>
</xsl:template>

<xsl:template match="xtf:surro">
  <!-- don't process the initial token, i.e., MIN or the like -->
  <xsl:apply-templates select="*[position()>1]"/>
</xsl:template>

<xsl:template match="gdl:w|n:w|xtf1:w">
<!--
  <xsl:choose>
    <xsl:when test="not(.//gdl:g[not(@sign='ed.removed')])">
       (no w if there are no unremoved graphemes)
    </xsl:when>
    <xsl:otherwise>
      <w>
	<xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
      </w>
    </xsl:otherwise>
  </xsl:choose>
 -->
 <w>
   <xsl:attribute name="ref"><xsl:value-of select="@xml:id"/></xsl:attribute>
   <xsl:copy-of select="@syn:*"/>
 </w>
</xsl:template>

<xsl:template match="gdl:gloss|xtf1:gloss">
  <d type="gloss-start" utype="{@type}"/>
    <xsl:apply-templates/>
  <d type="gloss-end"/>
</xsl:template>

<xsl:template match="gdl:p">
  <d type="punct"/>
</xsl:template>

<xsl:template match="xtf:l/xtf:mpx[1]">
  <xsl:apply-templates/>
</xsl:template>

<!--
 FIXME: this discards graphemes in the OGS part of the MPX.  A fix
 needs a little work because the g nodes here don't have an ID so they
 won't work as is with the XCL/XIM sequence.  The solution is either
 to wrap g nodes inside w nodes even in mpx[2] or to put IDs on the g
 nodes when they are top-level within mpx[2]. 
 -->
<xsl:template match="xtf:l[@type='ogs']"/>

<xsl:template 
    match="xtf:cmt|xtf:note|xtf1:note|xtf:protocols|xtf:protocol|gdl:nonw|xtf1:g"/>

<!-- ETCSL TEI legacy features -->

<xsl:template match="xtf1:s">
  <c type="block" subtype="tei.s" level="6"/>
</xsl:template>

<xsl:template match="xtf1:lvg|xtf1:wvg">
  <xsl:for-each select="*">
    <xsl:apply-templates/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xtf1:*">
  <xsl:message>xtf-XCL.xsl<xsl:value-of select="concat('@',ancestor::*[@xml:id][1]/@xml:id,'/')"/>: untrapped XTF1 element <xsl:value-of 
     select="local-name()"/></xsl:message>
</xsl:template>

<xsl:template match="xtf:*">
  <xsl:message>xtf-XCL.xsl<xsl:value-of select="concat('@',ancestor::*[@xml:id][1]/@xml:id,'/')"/>: untrapped XTF2 element <xsl:value-of 
     select="local-name()"/></xsl:message>
</xsl:template>

<xsl:template match="gdl:*">
  <xsl:message>xtf-XCL.xsl<xsl:value-of select="concat('@',ancestor::*[@xml:id][1]/@xml:id,'/')"/>: untrapped GDL element <xsl:value-of 
     select="local-name()"/></xsl:message>
</xsl:template>

<xsl:template mode="form" match="text()"/>

<xsl:template name="c-text">
  <xsl:variable name="level">
    <xsl:choose>
      <xsl:when test="local-name(.) = 'div'">
	<xsl:value-of select="1+count(ancestor::*[local-name() = 'div'])"/>
      </xsl:when>
      <xsl:otherwise>0</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <c type="text" level="{$level}">
    <xsl:copy-of select="@xml:id|@xml:lang"/>
    <xsl:attribute name="xml:lang">
      <xsl:value-of select="ancestor-or-self::*[@xml:lang][1]/@xml:lang"/>
    </xsl:attribute>
  </c>
</xsl:template>

<!-- generate a chunk marker to end the division and indicate via
     @level what the post-closing level should be 
 -->
<xsl:template name="c-text-end-div">
  <xsl:variable name="level">
    <xsl:choose>
      <xsl:when test="local-name(.) = 'div'">
	<xsl:value-of select="1+count(ancestor::*[local-name() = 'div'])"/>
      </xsl:when>
      <xsl:otherwise>0</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <c type="clear" level="{$level}">
    <xsl:copy-of select="@xml:id|@xml:lang"/>
    <xsl:attribute name="xml:lang">
      <xsl:value-of select="ancestor-or-self::*[@xml:lang][1]/@xml:lang"/>
    </xsl:attribute>
  </c>
</xsl:template>

<xsl:template name="d-text"> 
  <d type="text">
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="xml:lang">
      <xsl:value-of select="ancestor-or-self::*[@xml:lang][1]/@xml:lang"/>
    </xsl:attribute>
  </d>
</xsl:template>

<xsl:template name="div-start"> 
  <d dtype="div-start">
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="xml:lang">
      <xsl:value-of select="ancestor-or-self::*[@xml:lang][1]/@xml:lang"/>
    </xsl:attribute>
  </d>
</xsl:template>

<xsl:template name="div-end"> 
  <d dtype="div-end">
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="xml:lang">
      <xsl:value-of select="ancestor-or-self::*[@xml:lang][1]/@xml:lang"/>
    </xsl:attribute>
  </d>
</xsl:template>

<xsl:template match="xtf:include|xtf:referto"/>

</xsl:stylesheet>
