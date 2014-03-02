<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    version="1.0">

<xsl:param name="sort" select="'ogsl'"/>
<xsl:output method="xml" encoding="utf-8"/>

<xsl:template match="/*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="*">
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:attribute name="slnum">
	  <xsl:value-of select="1+count(preceding-sibling::*)"/>
	</xsl:attribute>
	<xsl:copy-of select="*"/>
      </xsl:copy>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

</xsl:transform>
