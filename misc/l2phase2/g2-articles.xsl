<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:isslp="http://oracc.org/ns/issl/1.0"
  xmlns:epad="http://psd.museum.upenn.edu/epad/"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="isslp">

<xsl:param name="basename"/>

<!--
<xsl:variable name="statsfile"
	      select="concat('/usr/local/oracc/xml/',
		      	     $basename,
			     '/stats.xml')"/>

<xsl:variable name="stats" select="document($statsfile)"/>
-->

<!--<xsl:import href="article-driver.xsl"/>-->
<xsl:strip-space elements="*"/>
<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:key name="ISSL" match="isslp:entry" use="@ref"/>
<xsl:key name="ETCSL" match="match" use="@cfgw"/>

<xsl:template match="cbd:entries">
  <articles>
    <xsl:copy-of select="@xml:lang"/>
    <xsl:copy-of select="@n"/>
    <xsl:copy-of select="@name"/>
    <xsl:copy-of select="@project"/>
    <xsl:attribute name="dc:title">
      <xsl:value-of select="concat(@project,' ',@xml:lang,' Glossary')"/>
    </xsl:attribute>
    <xsl:apply-templates/>
  </articles>
</xsl:template>

<xsl:template match="cbd:entry">
  <xsl:variable name="dcf">
    <xsl:choose>
      <xsl:when test="cbd:dcf[@xml:lang=/*/@xml:lang]">
	<xsl:value-of select="cbd:dcf[@xml:lang=/*/@xml:lang]/text()"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="cbd:cf/text()"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='dc:title')]"/>
    <xsl:attribute name="dc:title">
      <xsl:value-of select="concat(/*/@project, '/', /*/@xml:lang, '/', $dcf, '[', cbd:gw, ']')"/>
    </xsl:attribute>
    <xsl:apply-templates/>
<!--
    <xsl:call-template name="issl-prime"/>
    <xsl:call-template name="etcsl"/>
 -->
  </xsl:copy>
</xsl:template>

<xsl:template match="cbd:sense|cbd:subsense|cbd:subsubsense">
  <xsl:variable name="num">
    <xsl:number count="cbd:sense|cbd:subsense|cbd:subsubsense" level="multiple"/>
  </xsl:variable>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="num">
      <xsl:value-of select="$num"/>
      <xsl:if test="not(contains($num,'.'))"><xsl:text>.</xsl:text></xsl:if>
    </xsl:attribute>
<!--
    <xsl:variable name="id" select="@xml:id"/>
    <xsl:for-each select="$stats">
      <xsl:for-each select="id($id)">
	<xsl:attribute name="icount"><xsl:value-of select="@c"/></xsl:attribute>
	<xsl:attribute name="ipct"><xsl:value-of select="@pc"/></xsl:attribute>
      </xsl:for-each>
    </xsl:for-each>
 -->
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="cbd:equiv">
<!--  <xsl:variable name="id" select="@epad:external-id"/> -->
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:variable name="mng">
<!--
      <xsl:for-each 
       	    select="document('/usr/local//projects/epad/cbd/epad.cbd')">
        <xsl:value-of select="id($id)/cbd:sense/cbd:meaning"/>
      </xsl:for-each>
 -->
    </xsl:variable> 
    <term>
      <xsl:value-of select="text()"/>
    </term>
    <mean>
       <xsl:value-of select="$mng"/>
    </mean>
  </xsl:copy>
</xsl:template>

<xsl:template match="cbd:phon|cbd:t|cbd:bff|cbd:bff-listings">
  <xsl:copy-of select="."/>
<!--
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:call-template name="super-gloss"/>
  </xsl:copy>
 -->
</xsl:template>

<xsl:template match="cbd:t">
  <xsl:choose>
    <xsl:when test="ancestor::cbd:base">
      <text>
	<xsl:copy-of select="@*"/>
	<xsl:apply-templates/>
      </text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy-of select="."/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<!--
<xsl:template name="super-gloss">
  <xsl:param name="t" select="text()"/>
  <xsl:choose>
    <xsl:when test="contains($t,'{')">
      <xsl:value-of select="substring-before($t,'{')"/>
      <sup>
	<xsl:value-of select="substring-before(substring-after($t,'{'),
						'}')"/>
      </sup>
      <xsl:call-template name="super-gloss">
	<xsl:with-param name="t" select="substring-after($t,'}')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$t"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>
 -->

<xsl:template name="issl-prime">
  <xsl:variable name="id" select="@xml:id"/>
  <xsl:for-each select="document('../../issl/prime/issl-prime.xml',/)">
    <xsl:for-each select="key('ISSL',$id)">
      <bib>
	<xsl:for-each select="*">
	  <ref year="{@year}"><xsl:apply-templates/></ref>
	</xsl:for-each>
      </bib>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

<xsl:template name="etcsl">
  <xsl:variable name="cfgw" select="@dc:title"/>
  <xsl:for-each select="document('../../etcsl/lib/lemma+.xml',/)">
    <xsl:variable name="enodes" select="key('ETCSL',$cfgw)"/>
    <xsl:if test="count($enodes) > 0">
      <outlink type="corpus" name="ETCSL">
        <xsl:for-each select="$enodes">
          <xsl:variable name="e" select="../../etcsl"/>
  	    <link xl:href="{$e/href}">
  	      <xsl:value-of select="concat($e/@lemma,'=',$e/@label)"/>
  	    </link>
       </xsl:for-each>
     </outlink>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
