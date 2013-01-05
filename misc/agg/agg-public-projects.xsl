<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    version="1.0">

<xsl:strip-space elements="*"/>

<xsl:template match="/projects">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="*[xpd:public='yes']"/>
  </xsl:copy>
</xsl:template>

</xsl:transform>