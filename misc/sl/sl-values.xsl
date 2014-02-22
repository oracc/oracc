<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    version="1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="sl:v">
  <xsl:value-of select="concat(text(),'&#x9;', ancestor::sl:sign/@slnum, '&#xa;')"/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
