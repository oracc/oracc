<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:ex="http://exslt.org/common"
  exclude-result-prefixes="ex"
>

<xsl:template match="xh:div[@class='letter']">
  <xsl:variable name="letter" select="xh:h1[1]/text()"/>
  <xsl:variable name="p-nodes" select="xh:p[@class='summary'])"/>
  <xsl:value-of select="concat($letter,'&#x9;,count($p-nodes),'&#xa;')"/>
  <ex:document href="{concat('./',$webdir,$letter,'.ids')}"
	       method="text" encoding="utf-8">
    <xsl:for-each select="$p-nodes">
      <xsl:value-of select="concat(@id,'&#xa;')"/>;
    </xsl:for-each>
  </ex:document>
</xsl:template>

</xsl:stylesheet>
