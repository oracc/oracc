<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xis="http://oracc.org/ns/xis/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="lex">

<xsl:param name="xis-file" select="'word.xis'"/>

<xsl:key name="xis" match="xis:xis" use="."/>

<!--
<xsl:template match="/">
  <xsl:for-each select="document($xis-file,/)">
    <xsl:message><xsl:value-of select="key('xis','P382557.21.1+P382557.21.5')/@xml:id"/></xsl:message>
  </xsl:for-each>
</xsl:template>
-->

<xsl:template match="lex:group">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='xis')]"/>
    <xsl:if test="string-length(@xis)>0">
      <xsl:variable name="xis" select="@xis"/>
<!--      <xsl:message>trying <xsl:value-of select="$xis"/></xsl:message> -->
      <xsl:attribute name="xis">
	<xsl:for-each select="document($xis-file,/)">
	  <xsl:value-of select="key('xis',$xis)/@xml:id"/>
	</xsl:for-each>
      </xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
