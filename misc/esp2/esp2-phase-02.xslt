<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:esp="http://oracc.org/ns/esp/1.0"
	xmlns:param="http://oracc.org/ns/esp-param/1.0"
	xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
	xmlns="http://www.w3.org/1999/xhtml"
	version="2.0" 
	xpath-default-namespace="http://www.w3.org/1999/xhtml"
>
<xsl:output 
	method="xml" 
	encoding="utf-8"
	indent="no"
/>
<xsl:include href="esp2-functions.xslt"/>
<xsl:param name="output-file"/>
<xsl:param name="projesp"/>

<xsl:template match="/">
<!--	<xsl:message>Phase 2: Enhance and standardise source tree</xsl:message> -->
	<xsl:result-document href="{$output-file}"> 
		<xsl:apply-templates/>
	</xsl:result-document>
</xsl:template>

<!-- add url attributes to esp:pages -->
<xsl:template match="struct:page">
	<xsl:copy>
	  <xsl:if test="count(ancestor::struct:page)=0">
	    <xsl:variable name="parameters" select="document ( concat($projesp, '/00web/00config/parameters.xml') )/param:parameters"/>
	    <xsl:if test="$parameters/param:rootindex">
	      <xsl:attribute name="rootindex"><xsl:value-of select="$parameters/param:rootindex"/></xsl:attribute>
	    </xsl:if>
	  </xsl:if>
	    <xsl:if test="not(@url)">
		<xsl:attribute name="url">
			<xsl:text>/</xsl:text>
			<xsl:for-each select="ancestor-or-self::struct:page[ancestor::struct:page]">
			  <xsl:variable name="url-or-name">
			    <xsl:choose>
			      <xsl:when test="esp:url">
				<xsl:value-of select="esp:url"/>
			      </xsl:when>
			      <xsl:otherwise>
				<xsl:value-of select="esp:name"/>
			      </xsl:otherwise>
			    </xsl:choose>
			  </xsl:variable>
			  <xsl:variable name="processed-name"
					select="esp:alpha-only($url-or-name)"
					/>
			  <!--select="esp:make-alphanumeric ( $url-or-name )"-->
			  <xsl:value-of select="$processed-name"/>
			  <xsl:variable name="num-same-names" 
					select="count ( preceding-sibling::struct:page
						[esp:alpha-only(esp:name) = $processed-name] )"
					/>
			  <!--select="count ( preceding-sibling::struct:page
			      [esp:make-alphanumeric ( esp:name ) = $processed-name] )"-->
			  <xsl:if test="$num-same-names">
			    <xsl:value-of select="$num-same-names + 1"/>
			  </xsl:if>
			  <xsl:text>/</xsl:text>
			</xsl:for-each>
		</xsl:attribute>
	    </xsl:if>
	    <xsl:copy-of select="@*"/>
	    <xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

<!-- add glossary 'term' attributes -->
<xsl:template match="esp:glossary[not ( @term )]">
	<xsl:copy>
		<xsl:attribute name="term" select="text()"/>
		<xsl:copy-of select="@*"/>
		<xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

<!-- add technical terms 'term' attributes -->
<xsl:template match="esp:techterms[not ( @term )]">
	<xsl:copy>
		<xsl:attribute name="term" select="text()"/>
		<xsl:copy-of select="@*"/>
		<xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

<!-- add index 'term' and 'id' attributes -->
<xsl:template match="esp:index">
	<xsl:copy>
		<xsl:if test="not (@term)">
			<xsl:attribute name="term" select="text ()"/>
		</xsl:if>
		<xsl:attribute name="id" select="generate-id ()"/>
		<xsl:copy-of select="@*"/>
		<xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

<!-- copy the rest unchanged -->
<xsl:template match="*">
	<xsl:copy>
		<xsl:copy-of select="@*"/>
		<xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

</xsl:stylesheet>
