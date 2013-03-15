<xsl:stylesheet
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"	
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:esp="http://oracc.org/ns/esp/1.0"
	xmlns:refer="http://oracc.org/ns/esp-refer/1.0"
	xmlns="http://www.w3.org/1999/xhtml"
	version="2.0" 	
	xpath-default-namespace="http://www.w3.org/1999/xhtml"
>

<!-- FIXME: sjt: the implementation here with // is very slow;
     rewrite to use xsl:key -->

<xsl:variable name="referents" 
	      select="document ( concat($projesp,'/site-content/referents.xml') )"/>

<xsl:template name="references">
	<xsl:variable name="references" select=".//esp:ref"/>
	<xsl:if test="$references">
		<div id="References">
			<esp:bookmark id="references" hide-highlight="yes"/>
			<h2>References</h2>
			<ol>
				<xsl:for-each select="$references">
					<li id="highlight_ref_{position ()}">
						<esp:bookmark id="ref_{position ()}" hide-highlight="yes"/>
						<xsl:copy-of select="$referents//refer:referent[@id = current()/@referent]/node ()"/>
						<xsl:call-template name="page-numbers"/>
						<xsl:text> (</xsl:text>
						<esp:link bookmark="reflink_{position ()}" title="Jump back to reference number, above">Find in text ^</esp:link>
						<xsl:text>)</xsl:text>
					</li>
				</xsl:for-each>
			</ol>
		</div>
	</xsl:if>
	<xsl:variable name="further-readings" select=".//esp:reading"/>
	<xsl:if test="$further-readings">
		<div id="FurtherReading">
			<esp:bookmark id="furtherreading" hide-highlight="yes"/>
			<h2>Further reading</h2>
			<ul>
				<xsl:for-each select="$further-readings">
					<li>
						<xsl:copy-of select="$referents//refer:referent[@id = current()/@referent]/node ()"/>
						<xsl:call-template name="page-numbers"/>
					</li>
				</xsl:for-each>
			</ul>
		</div>
	</xsl:if>	
</xsl:template>

<xsl:template name="page-numbers">
	<xsl:if test="@page">
		<xsl:choose>
			<xsl:when test="string ( number ( @page ) ) = 'NaN'">
				<xsl:text>, pp. </xsl:text>
			</xsl:when>
			<xsl:otherwise>
			<xsl:text>, p. </xsl:text>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:value-of select="@page"/>
	</xsl:if>
	<xsl:text>.</xsl:text>
</xsl:template>

</xsl:stylesheet>
