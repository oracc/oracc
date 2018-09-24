<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:i="http://oracc.org/ns/instances/1.0"
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:note="http://oracc.org/ns/note/1.0"
  xmlns:n="http://oracc.org/ns/norm/1.0"
  xmlns:norm="http://oracc.org/ns/norm/1.0"
  xmlns:usg="http://oracc.org/ns/usg/1.0"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="dc i xl xcl norm usg note">

<xsl:output method="text"/>

<xsl:template match="cbd:entry[@oid]">  
  <xsl:value-of select="concat(@oid,'&#x9;',@xml:id,'&#xa;')"/>
</xsl:template>

<xsl:template match="text()"/>
</xsl:stylesheet>
