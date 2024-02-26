<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns="http://oracc.org/ns/sl/1.0"
    version="1.0">

<xsl:output method="xml" encoding="utf-8"/>

<xsl:template match="/*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="sl:sign[count(sl:sort)=0]">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <sort ogsl="100000" kwu="100000" mzl="100000"/>
    <xsl:copy-of select="*"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="sl:sign">
  <xsl:copy-of select="."/>
</xsl:template>

</xsl:transform>
