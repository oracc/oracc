<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"	
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:esp="http://oracc.org/ns/esp/1.0"
	xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
	xmlns="http://www.w3.org/1999/xhtml"
	version="2.0"
	xpath-default-namespace="http://www.w3.org/1999/xhtml"
>
<xsl:include href="esp2-chars-iso-8859-1.xslt"/>
<xsl:include href="esp2-chars-symbols-maths-greek.xslt"/>
<xsl:include href="esp2-chars-markup-internationalization.xslt"/>
<xsl:include href="esp2-dumb-quotes.xslt"/>

<!--	doctype-system="/xhtml1-strict.dtd"
	doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"  -->

<xsl:output	
	name="xhtml"
	method="xhtml" 
	encoding="utf-8"
	include-content-type="no"
	indent="no"
	media-type="text/html"
	omit-xml-declaration="yes"
	use-character-maps="iso-8859-1 symbols-maths-greek markup-internationalization dumb-quotes"
/>
<!-- 
	Notes:
	* omit-xml-declaration="yes" because the !DOCTYPE must be the first thing IE sees;
	* encoding="utf-8" because that (or utf-16, which causes problems) is required when no xml declaration is made;
	* include-content-type="no", because content-type in meta tags causes Netscape 4 to 'burp';
	* indent="no" is required to prevent layout problems in both IE and FireFox (and possibly others);
	* since all references to character-set are removed, the server should be set up to include this information in the content-type header.
-->
<xsl:param name="output-directory"/>

<xsl:template match="/">
<!--	<xsl:message>Phase 5: Split into pages (any warning refers to the file named *before* the warning)</xsl:message> -->
	<xsl:for-each select="//html">
		<xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
		<xsl:variable name="output-file">
			<xsl:value-of select="$output-directory"/>
			<xsl:value-of select="$current-page/@url"/>
			<xsl:choose>
			  <xsl:when test="ancestor::struct:page[1][@rootindex]">
			    <xsl:value-of select="ancestor::struct:page[1]/@rootindex"/>
			  </xsl:when>
			  <xsl:otherwise>
			    <xsl:text>index.html</xsl:text>
			  </xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:message>ESP processing <xsl:value-of select="$output-file"/></xsl:message>
		<xsl:result-document href="{$output-file}" format="xhtml">
			<xsl:copy copy-namespaces="no" exclude-result-prefixes="esp">
				<xsl:attribute name="xml:lang">en</xsl:attribute>
				<xsl:attribute name="lang">en</xsl:attribute>
				<xsl:comment> This page is automatically generated from XML data files. Please do not edit by hand. Instead, modify the source XML and regenerate. </xsl:comment>
				<xsl:apply-templates mode="page"/>
			</xsl:copy>
		</xsl:result-document>	
	</xsl:for-each>
	<xsl:apply-templates/>
</xsl:template>

<!-- convert esp:comments -->
<xsl:template match="esp:comment" mode="page">
	<xsl:comment><xsl:apply-templates mode="#current"/></xsl:comment>
</xsl:template>

<!-- strip and complain about any remaining esp:* tags -->
<xsl:template match="esp:*" mode="page">
	<xsl:message>	WARNING! Remaining esp: tag '<xsl:value-of select="name ()"/>'</xsl:message>
	<xsl:apply-templates mode="#current"/>
</xsl:template>

<!-- copy the rest unchanged -->
<xsl:template match="*" mode="page">
	<xsl:copy copy-namespaces="no">
		<xsl:copy-of select="@*"/>
		<xsl:apply-templates mode="#current"/>
	</xsl:copy>
</xsl:template>

<!-- ignore anything outside html tags -->
<xsl:template match="*"/>

</xsl:stylesheet>
