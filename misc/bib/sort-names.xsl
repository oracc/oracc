<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
		xmlns:tei="http://www.tei-c.org/ns/1.0"
		version="1.0">
<xsl:output mode="xml" indent="yes" encoding="utf-8"/>
<xsl:template match="/*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="*">
      <xsl:sort select="@key"/>
      <xsl:copy-of select="."/>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>