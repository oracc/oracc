<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:wm="http://oracc.org/ns/esp/1.0"
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
<xsl:include href="esp-functions.xslt"/>
<xsl:param name="output-file"/>

<xsl:template match="/">
	<xsl:message>
Phase 2: Enhance and standardise source tree</xsl:message>
	<xsl:result-document href="{$output-file}"> 
		<xsl:apply-templates/>
	</xsl:result-document>
</xsl:template>

<!-- add url attributes to wm:pages -->
<xsl:template match="struct:page">
	<xsl:copy>
	    <xsl:if test="not(@url)">
		<xsl:attribute name="url">
			<xsl:text>/</xsl:text>
			<xsl:for-each select="ancestor-or-self::struct:page[ancestor::struct:page]">
				<xsl:variable name="processed-name" select="wm:make-alphanumeric ( wm:name )"/>
				<xsl:value-of select="$processed-name"/>
				<xsl:variable name="num-same-names" 
					select="count ( preceding-sibling::struct:page
										[wm:make-alphanumeric ( wm:name ) = $processed-name] )"/>
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
<xsl:template match="wm:glossary[not ( @term )]">
	<xsl:copy>
		<xsl:attribute name="term" select="text()"/>
		<xsl:copy-of select="@*"/>
		<xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

<!-- add technical terms 'term' attributes -->
<xsl:template match="wm:techterms[not ( @term )]">
	<xsl:copy>
		<xsl:attribute name="term" select="text()"/>
		<xsl:copy-of select="@*"/>
		<xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

<!-- add index 'term' and 'id' attributes -->
<xsl:template match="wm:index">
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
