<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:esp="http://oracc.org/ns/esp/1.0"
	xmlns="http://www.w3.org/1999/xhtml"
	version="2.0" 
	xpath-default-namespace="http://www.w3.org/1999/xhtml"
>
<xsl:output	
	name="xml"
	method="xml" 
	encoding="utf-8"
	indent="yes"
/>

<xsl:param name="oracc" select="'/usr/local/oracc/'"/>
<xsl:param name="project"/>
<xsl:param name="projesp" select="concat($oracc, $project, '/00web/esp')"/>
<xsl:param name="scripts" select="concat($oracc,'lib/scripts')"/>
<xsl:param name="output-file"/>
<xsl:variable name="text-file" select="unparsed-text ( concat($projesp, '/temporary-files/images-info.txt'), 'utf-8' )"/>

<xsl:template match="/">
	<xsl:message>
Retrieving image dimensions</xsl:message>
	<xsl:result-document href="{$output-file}" format="xml">
		<esp:images-info>
			<xsl:analyze-string select="$text-file" regex=";[^\n]*site-content.00web.images.([^\n]+\.jpg|[^\n]+\.gif|[^\n]+\.png)\r?\n[^0-9]+Width \(pixels\):  ([0-9]+)[^0-9]+Height \(pixels\):  ([0-9]+)" flags="s">
				<xsl:matching-substring>
					<esp:image-info>
						<xsl:attribute name="file" select="translate ( regex-group ( 1 ), '\', '/' )"/>
						<xsl:attribute name="width" select="regex-group ( 2 )"/>
						<xsl:attribute name="height" select="regex-group ( 3 )"/>
					</esp:image-info>
				</xsl:matching-substring>
			</xsl:analyze-string>
		</esp:images-info>		
	</xsl:result-document>
</xsl:template>

</xsl:stylesheet>
