<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    version="1.0">

<xsl:strip-space elements="*"/>

<xsl:template match="/projects">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates select="*[xpd:public='yes']"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xpd:project">
  <xsl:if test="not(xpd:option[@name='project-list']/@value='no')">
    <xsl:copy-of select="."/>
  </xsl:if>
</xsl:template>

</xsl:transform>
