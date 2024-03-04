<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="sl:sign">
  <xsl:value-of select="concat(@xml:id,'&#x9;',ancestor::sl:letter/@xml:id,'&#xa;')"/>
  
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
