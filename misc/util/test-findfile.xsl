<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		version="1.0"
		>

<xsl:import href="findfile.xsl"/>

<xsl:output method="text"/>

<xsl:param name="project"/>
<xsl:param name="pqx"/>
<xsl:param name="ext"/>
<xsl:param name="code" select="'project'"/>
<xsl:param name="lang" select="'en'"/>

<xsl:template match="/">
  <xsl:call-template name="findfile">
    <xsl:with-param name="ff-project" select="$project"/>
    <xsl:with-param name="ff-pqx" select="$pqx"/>
    <xsl:with-param name="ff-ext" select="$ext"/>
  </xsl:call-template>
</xsl:template>

</xsl:stylesheet>
