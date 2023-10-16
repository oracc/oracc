<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:sl="http://oracc.org/ns/sl/1.0"
	       xmlns:g="http://oracc.org/ns/gdl/1.0"
	       xmlns:ex="http://exslt.org/common"
	       extension-element-prefixes="ex"
	       version="1.0">

<xsl:template match="sl:signlist">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="sl:iheader"/>
    <!--numbers are weeded it because they always have more than one child-->
    <xsl:for-each select="*/sl:sign">
      <xsl:variable name="k" select="sl:name/g:w//*"/>
      <xsl:if test="count($k)=1">
	<xsl:copy-of select="."/>
      </xsl:if>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
