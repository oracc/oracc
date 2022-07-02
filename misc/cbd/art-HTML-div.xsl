<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="1.0"
    xmlns:cbd="http://oracc.org/ns/cbd/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="cbd"
    >

<xsl:include href="xpd.xsl"/>

<xsl:template match="xh:body">
  <div>
    <xsl:copy-of select="*"/>
  </div>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
