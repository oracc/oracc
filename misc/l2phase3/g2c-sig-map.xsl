<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="utf-8" indent="no"/>

<xsl:template match="cbd:sig">
  <xsl:value-of select="concat(@sig,'&#x9;',
			ancestor::cbd:entry/@xml:id,'&#xa;')"/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
