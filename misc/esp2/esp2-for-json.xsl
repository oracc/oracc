<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    version="1.0" exclude-result-prefixes="xh">

<xsl:param name="project" select="''"/>

<xsl:template match="/">
  <text4json project="{$project}">
    <xsl:for-each select=".//xh:div[@id='Content']">
      <xsl:variable name="urlbase"
		    select="concat($project, ancestor::*/@url, 'index.html#')"/>
      <xsl:for-each select="*[string-length(text()) > 0]">
	<xsl:if test="not(@id='EndContentSpace')">
	  <chunk>
	    <type><xsl:value-of select="local-name(.)"/></type>
	    <url><xsl:value-of select="concat($urlbase,@id)"/></url>
	    <text>
	      <xsl:apply-templates mode="text"/>
	    </text>
	  </chunk>
	</xsl:if>
      </xsl:for-each>
    </xsl:for-each>
  </text4json>
</xsl:template>

<!--<xsl:template match="xh:span[@class='externallinktext']" mode="text"/>-->

<xsl:template match="*" mode="text">
  <xsl:apply-templates mode="text"/>
</xsl:template>

<xsl:template match="text()" mode="text">
  <xsl:value-of select="."/>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
